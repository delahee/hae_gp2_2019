// App.cpp : Ce fichier contient la fonction 'main'. L'exécution du programme commence et se termine à cet endroit.
//

#include "pch.h"
#include "Lib.hpp"
#include "Particle.h"
#include <iostream>
#include <SFML/Graphics.hpp>
#include <direct.h>
#include <functional>

#include <imgui-SFML.h>
#include <imgui.h>

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
		destFinal->draw(sprXY, _blurShader);
		destFinal->display();
	}

	
}

int main() {

	sf::ContextSettings settings;
	settings.antialiasingLevel = 2;
	
	sf::RenderWindow window(sf::VideoMode(1280, 720), "SFML works!", sf::Style::Default, settings);
	window.setVerticalSyncEnabled(true);


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

	sf::Texture whiteTex;
	if (!whiteTex.create(1, 1)) printf("tex crea failed\n");
	whiteTex.setSmooth(true);
	unsigned int col = 0xffffffff;
	whiteTex.update((const sf::Uint8*)&col,1,1,0,0);

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

	while (window.isOpen())//on passe tout le temps DEBUT DE LA FRAME 
	{
		sf::Event event;//recup les evenement clavier/pad
		frameStart = clock.getElapsedTime();
		while (window.pollEvent(event))	{

			ImGui::SFML::ProcessEvent(event);

			switch (event.type ) {
				case sf::Event::KeyReleased:
					
					break;

				case sf::Event::KeyPressed:
					{
					
					}
					break;

				case sf::Event::Closed:
					window.close();
					break;

				default:
					break;
			}
		}

		sf::RectangleShape sh(Vector2f(32, 32));
		sh.setPosition(16, 16);
		window.draw(sh);

		ImGui::SFML::Update(window, deltaClock.restart());

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

		window.clear( bgColor );//nettoie la frame
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

		

		srand(1055 + 100 +  1024 * 1024 + 0xdeadbeef);

		for (int i = 0; i < 150; ++i) {
			auto c = 2 + Lib::rd()*Lib::rd() * 2;
			sf::CircleShape shape(c, (int)(2 * 3.141569 * c));
			shape.setOrigin(Vector2f(c, c));
			shape.setPosition( 32 + Lib::rd() * 1920, Lib::rd() * 1080);
			sf::Color col = Lib::hsv( 190 + (int)(Lib::rd() * 30), 15.0/100.0 + Lib::rd() * 0.05, 95.0/100.0 + Lib::rd() * 0.1);
			shape.setFillColor(col);
			shape.setTexture(&whiteTex);
			window.draw(shape);
		}

		for (int i = 0; i < 20; ++i) {
			auto c = 12 + Lib::rd()*Lib::rd() * 250;
			sf::CircleShape shape(c, (int)(2 * 3.141569 * c));
			shape.setOrigin(Vector2f(c, c));
			shape.setPosition(32 + Lib::rd()  * 1920, Lib::rd() * 1080);
			shape.setFillColor(Lib::hsv(Lib::rd() *360.0, 0.9 + Lib::rd() * 0.01, 0.95 + Lib::rd() * 0.01));
			shape.setTexture(&whiteTex);
			window.draw(shape);
		}

		//draw palette
		if(false)
		for (int i = 0; i < 16; ++i) {
			int c = 32;
			sf::RectangleShape shape(Vector2f(c,c));
			shape.setPosition( c*2 * i + c * 0.5, 16);
			sf::Color col = Lib::hsv( (360.0 / 16) * i,0.9,0.9);
			shape.setFillColor(col);
			shape.setTexture(&whiteTex);
			window.draw(shape);
		}

		{
			auto c = 128;
			sf::CircleShape shape(c, (int)(2 * 3.141569 * c));
			shape.setOrigin(Vector2f(c, c));
			shape.setPosition(300, 300);
			shape.setFillColor(sf::Color(0xE884D4ff));
			shape.setTexture(&whiteTex);
			window.draw(shape);
		}

		{
			auto c = 32;
			sf::CircleShape shape(c, (int)(2 * 3.141569 * c));
			shape.setOrigin(Vector2f(c, c));
			shape.setPosition(600, 250);
			shape.setFillColor(sf::Color(0x33887Fff));
			shape.setTexture(&whiteTex);
			window.draw(shape);
		}

		{
			auto c = 48;
			sf::CircleShape shape(c, (int)(2 * 3.141569 * c));
			shape.setOrigin(Vector2f(c, c));
			shape.setPosition(150, 500);
			shape.setFillColor(sf::Color(0x337f09ff));
			shape.setTexture(&testTex);
			window.draw(shape);
		}

		

		{
			winTex.update(window);
			destX->clear(sf::Color(0, 0, 0, 255));
			destFinal->clear(sf::Color(0, 0, 0, 255));
			blur(blurWidth, &winTex, blurShader, destX,destFinal);
			sf::Sprite sp(destFinal->getTexture());
			sf::RenderStates rs;

			rs.blendMode = sf::BlendAdd;

			bloomShader->setUniform("texture", destFinal->getTexture());
			bloomShader->setUniform("bloomPass", 0.5f);
			bloomShader->setUniform("bloomMul", sf::Glsl::Vec4(1.3,1.3,1.0, 1.0) );

			rs.shader = bloomShader;
			sf::Color c = sp.getColor();
			c.a =(int)(c.a* 0.9);
			sp.setColor(c);

			window.draw(sp,rs);
			blurWidth += (1.0 / 60.0) * 2;

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
