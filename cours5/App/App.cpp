// App.cpp : Ce fichier contient la fonction 'main'. L'exécution du programme commence et se termine à cet endroit.
//

#include "pch.h"
#include "Lib.hpp"
#include "Particle.h"
#include <iostream>
#include <SFML/Graphics.hpp>
#include <direct.h>
#include <functional>

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
		float ofs = (offsetScale * (i - kernel_size * 0.5));
		_offsets[i].x = isHoriz ? ofs : 0.0f;
		_offsets[i].y = isHoriz ? 0.0f : ofs;
	}
}


static void blurX(float dx, sf::Texture* source, sf::Shader*_blurShader, sf::RenderTexture * destX) {
	source->setRepeated(true);
	destX->setRepeated(true);
	source->setSmooth(true);
	destX->setSmooth(true);
	{
		//blur the x
		_blurShader->setUniform("texture", *source);

		sf::Sprite sprX(*source);
		
		getKernelOffsets(dx, kernelX, offsetsX);

		int nbSamples = kernelX.size();
		_blurShader->setUniform("samples", nbSamples);
		for (int i = 0; i < nbSamples; i++)
			offsetsX[i].x *= 1.0f / source->getSize().x;
		_blurShader->setUniformArray("kernel", kernelX.data(), nbSamples);
		_blurShader->setUniformArray("offsets", offsetsX.data(), nbSamples);
		
		destX->draw(sprX, blurShader);
		destX->display();
	}
}


static void blur(float dx, sf::Texture* source, sf::Shader*_blurShader, sf::RenderTexture * destX, sf::RenderTexture * destFinal ) {
	source->setRepeated(true);
	destX->setRepeated(true);
	destFinal->setRepeated(true);

	source->setSmooth(true);
	destX->setSmooth(true);
	destFinal->setSmooth(true);
	{
		//blur the x
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


sf::Texture * bloomPass = nullptr;
int main() {

	sf::ContextSettings settings;
	settings.antialiasingLevel = 2;
	
	sf::RenderWindow window(sf::VideoMode(1280, 720), "SFML works!", sf::Style::Default, settings);
	window.setVerticalSyncEnabled(true);

	bloomPass = new sf::Texture();
	bloomPass->create(window.getSize().x, window.getSize().y);

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

	float blurWidth = 0.0f;

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


	while (window.isOpen())//on passe tout le temps DEBUT DE LA FRAME 
	{
		sf::Event event;//recup les evenement clavier/pad
		frameStart = clock.getElapsedTime();
		while (window.pollEvent(event))	{
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

		const int squareSpeed = 3;

		
		myFpsCounter.setPosition(8, 8);
		myFpsCounter.setFillColor(sf::Color::Red);
		myFpsCounter.setFont(*font);

		if (every == 0) {
			myFpsCounter.setString(std::string("FPS:") + std::to_string(fps[(step - 1) % 4]));
			every = 30;
		}
		every--;

		window.clear( sf::Color(0x00000000));//nettoie la frame
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

		sf::RectangleShape rsh;

		rsh.setFillColor(sf::Color(0xff00ffff));
		rsh.setSize(Vector2f(32, 32));
		rsh.setPosition(16, 50);

		window.draw(rsh);

		bloomPass->update(window);

		auto c = 128;
		sf::CircleShape shape(c, (int)(2 * 3.141569 * 100));
		shape.setOrigin(Vector2f(c, c));
		shape.setPosition(300, 300);
		shape.setFillColor(sf::Color(0xE884D4ff));
		shape.setTexture(&whiteTex);

		auto stex = shape.getTexture();
		
		window.draw(shape);

		{
			
			winTex.update(window);
			destX->clear(sf::Color(0, 0, 0, 0));
			destFinal->clear(sf::Color(0, 0, 0, 0));
			blur(blurWidth, &winTex, blurShader, destX,destFinal);
			sf::Sprite sp(destFinal->getTexture());
			sp.move(20, 20);
			window.draw(sp);
			blurWidth += (1.0 / 60.0) * 2;
		}
		
		window.display();//ca dessine et ca attend la vsync

		fps[step % 4] = 1.0f / (frameStart - prevFrameStart).asSeconds();
		prevFrameStart = frameStart;

		step++;
	}

	return 0;
}
