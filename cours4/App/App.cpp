// App.cpp : Ce fichier contient la fonction 'main'. L'exécution du programme commence et se termine à cet endroit.
//

#include "pch.h"
#include "Lib.hpp"
#include <iostream>
#include <SFML/Graphics.hpp>
#include <direct.h>

using namespace sf;

int main()
{
    std::cout << "Hello World!\n"; 

	sf::ContextSettings settings;
	settings.antialiasingLevel = 2;
	
	sf::RenderWindow window(sf::VideoMode(1280, 720), "SFML works!", sf::Style::Default, settings);
	sf::CircleShape shape(100.f, (int) (2 * 3.141569 * 100));
	shape.setPosition(30, 30);
	shape.setFillColor(sf::Color(0xE884D4ff) );
	shape.setOutlineThickness(4);
	shape.setOutlineColor(sf::Color(0xFF8A70ff));
	window.setVerticalSyncEnabled(true);

	sf::Clock clock;

	sf::Time appStart = clock.getElapsedTime();
	sf::Time frameStart = clock.getElapsedTime();
	sf::Time prevFrameStart = clock.getElapsedTime();

	float fps[4] = { 0.f,0.f,0.f,0.f };
	int step = 0;
	sf::Font * font = new sf::Font();
	MemFile f;
	if (font->loadFromFile("res/DejaVuSans.ttf") == false) {
		printf("no such font\n");
	}

	sf::Text myFpsCounter;
	int every = 0;
	while (window.isOpen())//on passe tout le temps DEBUT DE LA FRAME 
	{
		sf::Event event;//recup les evenement clavier/pad
		frameStart = clock.getElapsedTime();
		while (window.pollEvent(event))
		{
			switch (event.type ) {
				case sf::Event::KeyReleased :

					if (event.key.code == sf::Keyboard::I)
						printf("instant fps %f\n", fps[(step-1)%4]);

					if (event.key.code == sf::Keyboard::F)
						printf("fps %f\n", 0.25f*(fps[0] + fps[1] + fps[2] + fps[3]) );
					
					break;


				case sf::Event::Closed:
					window.close();
					break;
				default:
					break;
			}
		}
		
		myFpsCounter.setPosition(8, 8);
		myFpsCounter.setFillColor(sf::Color::Red);
		myFpsCounter.setFont(*font);

		if (every == 0) {
			myFpsCounter.setString(std::string("FPS:") + std::to_string(fps[(step - 1) % 4]));
			every = 30;
		}
		every--;

		window.clear();//nettoie la frame
		window.draw(shape);//on demande le dessin d' une forme
		window.draw(myFpsCounter);
		window.display();//ca dessine et ca attend la vsync

		fps[step % 4] = 1.0f / (frameStart - prevFrameStart).asSeconds();
		prevFrameStart = frameStart;

		step++;
	}

	return 0;
}
