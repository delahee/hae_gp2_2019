// App.cpp : Ce fichier contient la fonction 'main'. L'exécution du programme commence et se termine à cet endroit.
//

#include "pch.h"
#include <iostream>
#include <SFML/Graphics.hpp>

using namespace sf;

int main()
{
    std::cout << "Hello World!\n"; 

	sf::ContextSettings settings;
	settings.antialiasingLevel = 2;
	
	sf::RenderWindow window(sf::VideoMode(1280, 720), "SFML works!", sf::Style::Default, settings);
	sf::CircleShape shape(100.f, 2 * 3.141569 * 100);
	shape.setFillColor(sf::Color(0xE884D4ff) );
	shape.setOutlineThickness(4);
	shape.setOutlineColor(sf::Color(0xFF8A70ff));

	sf::Clock clock;

	sf::Time appStart = clock.GetElapsedTime();

	while (window.isOpen())//on passe tout le temps
	{
		//rapatrier 
		//frameStart
		sf::Event event;//recup les evenement clavier/pad
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
		}

		window.clear();//nettoie la frame
		window.draw(shape);//on demande le dessin d' une forme
		//beforeDisplay
		window.display();//ca dessine et ca attend la vsync
		//frameEnd

		//quand on appuie sur 'E'
		//printf appstart frame start frameend previousframeEnd beforedisplay
		// 
	}

	return 0;
}
