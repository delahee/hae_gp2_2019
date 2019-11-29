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

static sf::Texture * whiteTex = nullptr;
int main() {

	sf::ContextSettings settings;
	settings.antialiasingLevel = 2;
	
	sf::RenderWindow window(sf::VideoMode(1280, 720), "SFML works!", sf::Style::Default , settings);
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
	
	whiteTex = new Texture();
	if (!whiteTex->create(1, 1)) printf("tex crea failed\n");
	whiteTex->setSmooth(true);
	unsigned int col = 0xffffffff;
	whiteTex->update((const sf::Uint8*)&col,1,1,0,0);

	sf::Text myFpsCounter;
	int every = 0;

	sf::Clock deltaClock;

	double winWidth = window.getSize().x;
	double winHeight = window.getSize().y;

	int showSegment = 0;

	while (window.isOpen())//on passe tout le temps DEBUT DE LA FRAME 
	{
		sf::Event event;//recup les evenement clavier/pad
		frameStart = clock.getElapsedTime();
		window.setView(initialView);

		window.clear();
		while (window.pollEvent(event))	{

			ImGui::SFML::ProcessEvent(event);

			Vector2f mousePos = sf::Vector2f(sf::Mouse::getPosition(window));

			switch (event.type ) {

				case sf::Event::Resized:
					initialView.setSize({
									   static_cast<float>(event.size.width),
									   static_cast<float>(event.size.height)
						});
					window.setView(initialView);
					break;

				case sf::Event::KeyReleased:
					
					break;

				case sf::Event::KeyPressed:
					
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

		myFpsCounter.setPosition(8, 8);
		myFpsCounter.setFillColor(sf::Color::Red);
		myFpsCounter.setFont(*font);



		if (every == 0) {
			myFpsCounter.setString(std::string("FPS:") + std::to_string(fps[(step - 1) % 4]));
			every = 30;
		}
		every--;

		window.draw(myFpsCounter);

		sf::RectangleShape sh(Vector2f(128,128));
		sh.setPosition(256, 256);
		sh.setTexture(whiteTex);
		simpleShader->setUniform("time", clock.getElapsedTime().asSeconds());
		window.draw(sh,simpleShader);


		ImGui::SFML::Render(window);
		window.display();//ca dessine et ca attend la vsync

		fps[step % 4] = 1.0f / (frameStart - prevFrameStart).asSeconds();
		prevFrameStart = frameStart;

		step++;
	}

	ImGui::SFML::Shutdown();

	return 0;
}

