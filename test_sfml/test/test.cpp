// test.cpp : Ce fichier contient la fonction 'main'. L'exécution du programme commence et se termine à cet endroit.
//

#include "pch.h"

#include "Lib.h"
#include <iostream>

#include <SFML/Graphics.hpp>

using namespace sf;

int main()
{
    std::cout << "Hello World!\n"; 

	sf::RenderWindow window(sf::VideoMode(800, 600), "SFML works!");
	//sf::CircleShape shape(100.f);
	//shape.setFillColor(sf::Color::Green);

	sf::Clock	clock;

	sf::Time	appStart = clock.getElapsedTime();
	while (window.isOpen())
	{
		sf::Time now = clock.getElapsedTime();
		sf::Time dt = appStart - now;

		if (dt.asSeconds() < 0.001f ) dt = sf::seconds(0.001f);

		sf::Event event;

		sf::VertexArray line(sf::LinesStrip);
		line.append(Vertex(Vector2f(10,10), sf::Color(255,0,0)));
		line.append(Vertex(Vector2f(100,100)));
		line.append(Vertex(Vector2f(100,50)));


		sf::VertexArray curve(sf::LinesStrip);

		std::vector<float> lx;
		lx.push_back(50);
		lx.push_back(75);
		lx.push_back(120);
		lx.push_back(180);

		std::vector<float> ly;
		ly.push_back(60);
		ly.push_back(85);
		ly.push_back(100);
		ly.push_back(280);

		Color red(240, 80, 40);

		for (int i = 0; i < 50; i++) {
			curve.append(Vertex(
				Vector2f(	Lib::plot(i / 50.0f, lx),
							Lib::plot(i / 50.0f, ly )),
				red));
		}
		
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
		}

		window.clear();
		window.draw(line);

		window.draw(curve);
		window.display();
	}

	return 0;
}

// Exécuter le programme : Ctrl+F5 ou menu Déboguer > Exécuter sans débogage
// Déboguer le programme : F5 ou menu Déboguer > Démarrer le débogage

// Conseils pour bien démarrer : 
//   1. Utilisez la fenêtre Explorateur de solutions pour ajouter des fichiers et les gérer.
//   2. Utilisez la fenêtre Team Explorer pour vous connecter au contrôle de code source.
//   3. Utilisez la fenêtre Sortie pour voir la sortie de la génération et d'autres messages.
//   4. Utilisez la fenêtre Liste d'erreurs pour voir les erreurs.
//   5. Accédez à Projet > Ajouter un nouvel élément pour créer des fichiers de code, ou à Projet > Ajouter un élément existant pour ajouter des fichiers de code existants au projet.
//   6. Pour rouvrir ce projet plus tard, accédez à Fichier > Ouvrir > Projet et sélectionnez le fichier .sln.
