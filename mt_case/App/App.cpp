// App.cpp : Ce fichier contient la fonction 'main'. L'exécution du programme commence et se termine à cet endroit.
//


#include <iostream>
#include <SFML/Graphics.hpp>
#include <direct.h>
#include <functional>

#include "Lib.hpp"
#include "Game.hpp"
#include "Particle.h"
#include "FadingParticle.h"
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

static Game g;

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



static Vector2f p0;
static Vector2f p1;

static RectangleShape shp;

static RectangleShape walls[4];

int main() {

	sf::ContextSettings settings;
	settings.antialiasingLevel = 2;
	
	sf::RenderWindow window(sf::VideoMode(1280, 720), "SFML works!", sf::Style::Default , settings);
	window.setVerticalSyncEnabled(true);

	sf::View initialView = window.getDefaultView();
	sf::View mainView = window.getDefaultView();
	window.setView(mainView);

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

	int showSegment = 0;

	sf::Color c(0xE88A383f);
	walls[0].setFillColor(c);
	walls[0].setPosition(-15, 0);
	walls[0].setSize(Vector2f(16, winHeight));

	walls[1].setFillColor(c);
	walls[1].setPosition(winWidth - 1, 0);
	walls[1].setSize(Vector2f(16, winHeight));

	walls[2].setFillColor(c);
	walls[2].setPosition(0, -15);
	walls[2].setSize(Vector2f(winWidth, 16));

	walls[3].setFillColor(c);
	walls[3].setPosition(0, winHeight - 1);
	walls[3].setSize(Vector2f(winWidth, 16));

	while (window.isOpen())//on passe tout le temps DEBUT DE LA FRAME 
	{
		sf::Event event;//recup les evenement clavier/pad
		frameStart = clock.getElapsedTime();
		window.setView(initialView);

		while (window.pollEvent(event))	{

			Vector2f mousePos = sf::Vector2f(sf::Mouse::getPosition(window));

			switch (event.type ) {

				case sf::Event::MouseMoved :
				{
					int life = 30;
					float sp = Lib::rd() + 0.5;
					{
						auto c = new CircleShape(8, 64);
						c->setOrigin(8, 8);
						c->setPosition(mousePos);
						c->setFillColor(sf::Color(0xFF5C371a));
						auto p = new FadingParticle(c);
						p->life = 30;
						p->speed.y = sp;
						p->bhv = Particle::applySpeed;
						g.pvec.push_back(p);
						
					}

					{
						auto c = new CircleShape( 2,32);
						c->setOrigin(2, 2);
						c->setPosition(mousePos);
						c->setFillColor(sf::Color(0xE88A38ff));

						auto p = new FadingParticle(c);
						p->life = 30;
						p->speed.y = sp;
						p->bhv = Particle::applySpeed;
						g.pvec.push_back(p);
					}
					
					break;
				}

				case sf::Event::Resized:
					initialView.setSize({
									   static_cast<float>(event.size.width),
									   static_cast<float>(event.size.height)
						});
					window.setView(initialView);
					winTex.create(window.getSize().x, window.getSize().y);
					delete destX;
					destX = new sf::RenderTexture();
					destX->create(window.getSize().x, window.getSize().y);
					destX->clear(sf::Color(0, 0, 0, 0));
					delete destFinal;
					destFinal = new sf::RenderTexture();
					destFinal->create(window.getSize().x, window.getSize().y);
					destFinal->clear(sf::Color(0, 0, 0, 0));
					break;

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

		const int squareSpeed = 3;

		myFpsCounter.setPosition(8, 8);
		myFpsCounter.setFillColor(sf::Color(0xE88A387f));
		myFpsCounter.setFont(*font);

		if (every == 0) {
			myFpsCounter.setString(std::string("FPS:") + std::to_string(fps[(step - 1) % 4]));
			every = 30;
		}
		every--;

		g.update(dt.asSeconds());
		al.update(dt.asSeconds());

		window.clear( sf::Color(3,8,10) );//nettoie la frame

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

		g.draw(window);

		for (int i = 0; i < 4; ++i) {
			window.draw(walls[i]);
		}

		window.draw(myFpsCounter);

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
		
		window.display();//ca dessine et ca attend la vsync

		fps[step % 4] = 1.0f / (frameStart - prevFrameStart).asSeconds();
		prevFrameStart = frameStart;

		step++;
	}


	return 0;
}
