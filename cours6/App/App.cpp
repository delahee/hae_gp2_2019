// App.cpp : Ce fichier contient la fonction 'main'. L'exécution du programme commence et se termine à cet endroit.
//

#include "pch.h"

#include <iostream>
#include <SFML/Graphics.hpp>
#include <direct.h>
#include <functional>

#include <imgui-SFML.h>
#include <imgui.h>

#include "Lib.hpp"
#include "Particle.h"
#include "Action.hpp"
#include <Box2D/Box2D.h>

using namespace sf;

static sf::Shader * simpleShader = nullptr;
static sf::Shader * redShader = nullptr;
static sf::Shader * bloomShader = nullptr;
static sf::Shader * blurShader = nullptr;

static Vector2f mousePos[4] = {
	Vector2f(0,0),
	Vector2f(100,100),
	Vector2f(200,200),
	Vector2f(300,800)
};

static std::vector<float>kernel;
static std::vector<sf::Glsl::Vec2>offsets;

static std::vector<float>kernelX;
static std::vector<sf::Glsl::Vec2>offsetsX;

static std::vector<float>kernelY;
static std::vector<sf::Glsl::Vec2>offsetsY;

//offset must be normalized by texture size
static void getKernelOffsets( float dx, std::vector<float> & _kernel, std::vector<sf::Glsl::Vec2> & _offsets, float offsetScale = 1.0f,bool isHoriz = true) {
	int kernel_size = (int)(dx / 0.65f + 0.5f) * 2 + 1;

	_kernel.clear();
	_offsets.clear();

	for (int i = 0; i < kernel_size; i++) {
		_kernel.push_back(0.0f);
		_offsets.push_back(sf::Glsl::Vec2());
	}

	Lib::m_gaussian_kernel(_kernel.data(), kernel_size, dx);

	for (int i = 0; i < kernel_size; i++) {
		float ofs = (offsetScale * (i - kernel_size * 0.5f));
		_offsets[i].x = isHoriz ? ofs : 0.0f;
		_offsets[i].y = isHoriz ? 0.0f : ofs;
	}
}

static void blur(float dx, sf::Texture* source, sf::Shader*_blurShader, sf::RenderTexture * destX, sf::RenderTexture * destFinal ) {

	source->setSmooth(true);
	destX->setSmooth(true);
	destFinal->setSmooth(true);
	{
		_blurShader->setUniform("texture", *source);

		sf::Sprite sprX(*source);

		getKernelOffsets(dx, kernelX, offsets);

		int nbSamples = kernelX.size();
		_blurShader->setUniform("samples", nbSamples);
		for (int i = 0; i < nbSamples; i++)
			offsets[i].x *= 1.0f / source->getSize().x;
		_blurShader->setUniformArray("kernel", kernelX.data(), nbSamples);
		_blurShader->setUniformArray("offsets", offsets.data(), nbSamples);

		auto k = 1.0f;
		_blurShader->setUniform("srcMul", sf::Glsl::Vec4(k, k, k, 1.0f));

		destX->draw(sprX, _blurShader);
		destX->display();
	}

	{
		getKernelOffsets(dx, kernelY, offsets,1.0,false);
		int nbSamples = kernelY.size();
		_blurShader->setUniform("samples", nbSamples);
		for (int i = 0; i < nbSamples; i++)
			offsets[i].y *= 1.0f / source->getSize().y;
		_blurShader->setUniformArray("kernel", kernelY.data(), nbSamples);
		_blurShader->setUniformArray("offsets", offsets.data(), nbSamples);

		sf::Sprite sprXY( destX->getTexture());
		_blurShader->setUniform("texture", destX->getTexture());
		_blurShader->setUniform("srcMul", sf::Glsl::Vec4(1, 1, 1, 1));

		destFinal->draw(sprXY, _blurShader);
		destFinal->display();
	}
}

static sf::Texture * whiteTex = nullptr;

class FadingParticle : public Particle {
public:
	int fadeStart = 20;

	FadingParticle(sf::Shape * s) : Particle(s){
		
	}

	virtual void update() {
		Particle::update();
		if (life < fadeStart) {
			auto sc = spr->getScale();
			spr->setScale(Vector2f(sc.x * 0.95, sc.y * 0.95));
		}
	}

};

static Vector2f p0;
static Vector2f p1;

static RectangleShape shp;

static RectangleShape walls[5];

int main() {

	sf::ContextSettings settings;
	settings.antialiasingLevel = 2;
	
	sf::RenderWindow window(sf::VideoMode(1280, 720), "SFML works!", sf::Style::Default, settings);
	window.setVerticalSyncEnabled(true);

	sf::View initialView = window.getDefaultView();
	sf::View mainView = window.getDefaultView();
	window.setView(mainView);
	ImGui::SFML::Init(window);

	sf::Clock clock;

	sf::Time appStart = clock.getElapsedTime();
	sf::Time frameStart = clock.getElapsedTime();
	sf::Time prevFrameStart = clock.getElapsedTime();

	float fps[4] = { 0.f,0.f,0.f,0.f };
	int step = 0;
	sf::Font * font = new sf::Font();
	if (font->loadFromFile("res/DejaVuSans.ttf") == false) {
		printf("no such font\n");
	}

	if (!sf::Shader::isAvailable())
		printf("no shader avail\n");

	simpleShader = new Shader();
	if (!simpleShader->loadFromFile("res/simple.vert", "res/simple.frag"))
		printf("unable to load shaders\n");

	redShader = new Shader();
	if (!redShader->loadFromFile("res/simple.vert", "res/red.frag"))
		printf("unable to load shaders\n");

	bloomShader = new Shader();
	if (!bloomShader->loadFromFile("res/simple.vert", "res/bloom.frag"))
		printf("unable to load shaders\n");

	blurShader = new Shader();
	if (!blurShader->loadFromFile("res/simple.vert", "res/blur.frag"))
		printf("unable to load shaders\n");
	
	std::vector< Particle * > vec;

	
	whiteTex = new Texture();
	if (!whiteTex->create(1, 1)) printf("tex crea failed\n");
	whiteTex->setSmooth(true);
	unsigned int col = 0xffffffff;
	whiteTex->update((const sf::Uint8*)&col,1,1,0,0);

	sf::Texture testTex;
	//testTex.create(256, 256);
	if (!testTex.loadFromFile("res/test3.png")) {
		printf("tex crea 2 failed\n");
	}
	testTex.setSmooth(true);

	float blurWidth = 50.0f;

	sf::Text myFpsCounter;
	int every = 0;

	sf::RenderTexture  * destX = new sf::RenderTexture();
	destX->create(window.getSize().x, window.getSize().y);
	destX->clear(sf::Color(0, 0, 0, 0));

	sf::RenderTexture * destFinal = new sf::RenderTexture();
	destFinal->create(window.getSize().x, window.getSize().y);
	destFinal->clear(sf::Color(0, 0, 0, 0));

	sf::Texture winTex;
	winTex.create(window.getSize().x, window.getSize().y);

	sf::Clock deltaClock;

	sf::Color bgColor(7,15,33);
	char windowTitle[256] = "maWindow";
	float color[3] = { bgColor.r/255.0f, bgColor.g / 255.0f, bgColor.b / 255.0f };

	ActionList al;
	double winWidth = window.getSize().x;
	double winHeight = window.getSize().y;

	sf::Color c(0xE81359ff);
	walls[0].setFillColor(c);
	walls[0].setPosition( -15,0);
	walls[0].setSize(Vector2f(16, winHeight));

	walls[1].setFillColor(c);
	walls[1].setPosition(winWidth-1, 0);
	walls[1].setSize(Vector2f(16, winHeight));

	walls[2].setFillColor(c);
	walls[2].setPosition(0, -15);
	walls[2].setSize(Vector2f(winWidth, 16));

	walls[3].setFillColor(c);
	walls[3].setPosition(0, winHeight-1);
	walls[3].setSize(Vector2f(winWidth, 16));

	al.push(new Delay([winWidth, winHeight,&vec]() {
		int nb = 128;
		for (int i = 0; i <nb; ++i) {
			Shape* sh = new sf::CircleShape(16);
			sh->setOrigin(16, 16);
			//auto angle = Lib::rd() * 3.14156;
			auto angle = 1.0 * i / nb * 3.14156 * 2;
			float dx = cos(angle) * 180.0f;
			float dy = sin(angle) * 180.0f;
			Particle * p = new Particle(sh);
			p->life = 1000000;

			sh->setPosition(winWidth*0.5 + dx, winHeight * 0.5 + dy);
			p->speed.x = dx * 0.05;
			p->speed.y = dy * 0.05;

			sh->setFillColor(sf::Color(200,0,0,255));

			p->bhv = [](Particle* p) {

				walls[4].setPosition(shp.getPosition());
				walls[4].setSize(shp.getSize());
				auto pos = p->spr->getPosition();
				auto speed = p->speed;

				auto nextPos = pos + speed;

				for (int k = 0; k < 5; ++k) {
					b2Vec2 inter;
					b2Vec2 normal;
					//printf("speed %f %f\n", speed.x, speed.y);
					if (Lib::willCollide(pos, speed, &walls[k], inter, normal)) {
						b2Vec2 startToInter = inter - b2Vec2(pos.x, pos.y);
						b2Vec2 refl = startToInter - 2 * Lib::dot(startToInter, normal) * normal;
						b2Vec2 endRefl = inter + refl;

						auto oldPos = pos;
						//pos = Vector2f(endRefl.x, endRefl.y);

						Vector2f reflNorm(refl.x, refl.y);
						Lib::v2Norm(reflNorm);

						Vector2f outSpeed(reflNorm.x, reflNorm.y);
						double vlen = Lib::v2Len(speed);
						outSpeed.x *= vlen;
						outSpeed.y *= vlen;

						p->speed = outSpeed;
						pos = Vector2f(endRefl.x, endRefl.y);
						speed = Vector2f(0,0);
						nextPos = pos + speed;
					}
					else {
						//nextPos = pos + speed;
					}
				}
				p->spr->setPosition(nextPos);
			};
			vec.push_back(p);
		}
	}, 800));

	int showSegment = 0;

	while (window.isOpen())//on passe tout le temps DEBUT DE LA FRAME 
	{
		sf::Event event;//recup les evenement clavier/pad
		frameStart = clock.getElapsedTime();
		window.setView(initialView);

		while (window.pollEvent(event))	{

			ImGui::SFML::ProcessEvent(event);

			Vector2f mousePos = sf::Vector2f(sf::Mouse::getPosition(window));

			switch (event.type ) {
				case sf::Event::KeyReleased:
					
					break;

				case sf::Event::KeyPressed:
					if (event.key.code == sf::Keyboard::F1) {
						p0.x = mousePos.x;
						p0.y = mousePos.y;
						showSegment++;
					}

					if (event.key.code == sf::Keyboard::F2) {
						p1.x = mousePos.x;
						p1.y = mousePos.y;
						showSegment++;
					}

					if (event.key.code == sf::Keyboard::Space) {
						mainView = initialView;
						mainView.move(Vector2f(Lib::dice(-8, 8), Lib::dice(-8, 8)));
						window.setView(mainView);
					}
					break;

				case sf::Event::Closed:
					window.close();
					break;

				default:
					break;
			}
		}

		

		sf::Time dt = deltaClock.restart();
		ImGui::SFML::Update(window, dt);

		const int squareSpeed = 3;

		myFpsCounter.setPosition(8, 8);
		myFpsCounter.setFillColor(sf::Color::Red);
		myFpsCounter.setFont(*font);

		if (every == 0) {
			myFpsCounter.setString(std::string("FPS:") + std::to_string(fps[(step - 1) % 4]));
			every = 30;
		}
		every--;

		ImGui::Begin("Sample window"); // begin window
		// Background color edit
		if (ImGui::ColorEdit3("Background color", color)) {
			// this code gets called if color value changes, so
			// the background color is upgraded automatically!
			bgColor.r = static_cast<sf::Uint8>(color[0] * 255.f);
			bgColor.g = static_cast<sf::Uint8>(color[1] * 255.f);
			bgColor.b = static_cast<sf::Uint8>(color[2] * 255.f);
		}

		// Window title text edit
		ImGui::InputText("Window title", windowTitle, 255);

		if (ImGui::Button("Update window title")) {
			// this code gets if user clicks on the button
			// yes, you could have written if(ImGui::InputText(...))
			// but I do this to show how buttons work :)
			window.setTitle(windowTitle);
		}
		ImGui::End(); // end window

		al.update(dt.asSeconds());

		auto cl15 = fabs(fmodf(clock.getElapsedTime().asSeconds(), 0.5f));
		if (cl15 >= 0.0 && cl15 <= 0.25) {
			bgColor.r = static_cast<sf::Uint8>(color[0] * 255.f) + 32;
		}
		else 
			bgColor.r = static_cast<sf::Uint8>(color[0] * 255.f);

		window.clear( bgColor );//nettoie la frame

		/////////////
		/////////////
		///DRAW START

		if (showSegment >= 2) {

			shp.setSize(Vector2f(100, 100));
			shp.setOrigin(0, 0);
			shp.setPosition(150, 400);
			shp.setFillColor(sf::Color(180, 100, 90, 255));
			shp.setOutlineThickness(2.0);
			shp.setOutlineColor(sf::Color(200, 200, 200, 255));

			window.draw(shp);

			double len = Lib::v2Len(p1 - p0);
			sf::RectangleShape sh(Vector2f(1, len));
			sh.setFillColor(sf::Color(255, 0, 127, 255));
			sh.setOrigin(0.5, 0.0);
			sh.setPosition(p0.x, p0.y);
			double angle = atan2(p1.y - p0.y, p1.x - p0.x);
			angle -= Lib::PI * 0.5;
			sh.setRotation( angle / (2.0 * 3.14156) * 360.0);
			window.draw(sh);

			sf::RectangleShape origin;
			origin.setOrigin(2, 2);
			origin.setSize(Vector2f(4, 4));
			origin.setPosition(p0.x, p0.y);
			window.draw(origin);
			
			Vector2f speed = p1 - p0;
			b2Vec2 inter;
			b2Vec2 normal;
			if (Lib::willCollide(p0, speed, &shp, inter, normal)) {
				auto dotSize = 8;
				sf::RectangleShape lp;
				lp.setOrigin(dotSize*0.5, dotSize*0.5);
				lp.setSize(Vector2f(dotSize, dotSize));
				lp.setFillColor(sf::Color(0, 255, 0, 255));
				lp.setPosition(inter.x, inter.y);
				window.draw(lp);

				{
					sf::VertexArray line(sf::LineStrip);
					line.append(sf::Vertex(Vector2f(inter.x, inter.y), sf::Color(255, 0, 0, 255)));
					line.append(sf::Vertex(Vector2f(inter.x + normal.x * 30, inter.y + normal.y * 30), sf::Color(255, 0, 0, 255)));
					window.draw(line);
				}

				b2Vec2 startToInter = inter - b2Vec2(p0.x, p0.y);
				b2Vec2 refl = startToInter - 2 * Lib::dot(startToInter, normal) * normal;
				b2Vec2 endRefl = inter + refl;

				{
					sf::VertexArray line(sf::LineStrip);
					line.append(sf::Vertex(Vector2f(inter.x, inter.y), sf::Color(255, 255, 0, 255)));
					line.append(sf::Vertex(Vector2f(endRefl.x, endRefl.y), sf::Color(255, 255, 0, 255)));
					window.draw(line);
				}
			}
		}

		for (int i = 0; i < 4; ++i) {
			window.draw(walls[i]);
		}

		window.draw(myFpsCounter);

		for (int k = 0; k < (int)vec.size(); k++) {
			Particle * p = vec[vec.size() - k - 1];
			p->update();
			if (p->killed) {
				vec.erase(vec.begin()+k);
			}
			else {
				p->draw(window);
			}
		}

		///Draw all bloomed before this
		{
			winTex.update(window);
			destX->clear(sf::Color(0, 0, 0, 255));
			destFinal->clear(sf::Color(0, 0, 0, 255));
			blur(blurWidth, &winTex, blurShader, destX,destFinal);
			sf::Sprite sp(destFinal->getTexture());
			sf::RenderStates rs;

			rs.blendMode = sf::BlendAdd;

			bloomShader->setUniform("texture", destFinal->getTexture());
			bloomShader->setUniform("bloomPass", 0.6f);
			bloomShader->setUniform("bloomMul", sf::Glsl::Vec4(1.3f,1.3f,1.0f, 1.0f) );

			rs.shader = bloomShader;
			sf::Color c = sp.getColor();
			c.a =(int)(c.a* 0.8);
			sp.setColor(c);

			window.draw(sp,rs);
			blurWidth += (1.0f / 60.0f) * 2;

			if (blurWidth >= 64)
				blurWidth = 54;
		}
		
		ImGui::SFML::Render(window);
		window.display();//ca dessine et ca attend la vsync

		fps[step % 4] = 1.0f / (frameStart - prevFrameStart).asSeconds();
		prevFrameStart = frameStart;

		step++;
	}

	ImGui::SFML::Shutdown();

	return 0;
}
