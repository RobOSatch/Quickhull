// Quickhull.cpp : This file contains the 'main' function. Program execution begins and ends there.

#include <iostream>
#include <vector>
#include <stdlib.h>
#include <time.h>

#include <SFML/Graphics.hpp>
#include <glm/glm.hpp>
#include <glm/gtx/closest_point.hpp>

#include <chrono>

#include <string>
#include <fstream>
#include <algorithm>

#include "Timer.h"
#include "convexHull.h"


// CONSTANTS
const int SCREEN_WIDTH = 1920;
const int SCREEN_HEIGHT = 1080;
const int n = 100;

int step = 0;

// Renders the appropriate hull step
void update(sf::RenderWindow& window, std::vector<sf::Vertex> &points)
{
	window.clear();

	// Draw points
	for (sf::Vertex v : points) {
		sf::CircleShape shape(2.f);
		shape.setPosition(v.position);
		window.draw(shape);
	}

	if (step < 0) step = 0;
	if (step >= convex_hull::hullHistory.size()) step = convex_hull::hullHistory.size() - 1;

	std::vector<sf::Vertex> hullAtStep = convex_hull::hullHistory[step];
	window.draw(&hullAtStep[0], hullAtStep.size(), sf::LinesStrip);
}

// Imports floats from a file and returns them in a vector
std::vector<sf::Vertex> importPointsFromFile(std::string filename)
{
	std::string line;
	std::ifstream myfile;
	myfile.open(filename);

	std::vector<sf::Vertex> points;

	if (myfile.is_open())
	{
		std::getline(myfile, line);
		int numberCount = std::stoi(line);

		while (std::getline(myfile, line))
		{
			float x = std::stof(line.substr(0, line.find(',')));
			float y = std::stof(line.substr(line.find(',') + 1, line.length()));

			points.push_back(sf::Vertex(sf::Vector2f(x, y)));
		}

	}

	return points;
}

int main()
{	
	srand(time(NULL));
	std::vector<sf::Vertex> points;
	
	for (int i = 0; i < n; i++) {
		float x = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (SCREEN_WIDTH * 0.95))) + SCREEN_WIDTH * 0.025;
		float y = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (SCREEN_HEIGHT * 0.95))) + SCREEN_HEIGHT * 0.025;
		points.push_back(sf::Vertex(sf::Vector2f(x, y), sf::Color::Magenta));
	}

	std::vector<sf::Vertex> convexHull;

	switch (mode) {
	case eModePerformance:
		Timer::start();
		convexHull = convex_hull::quickHull(points);
		Timer::stop();

		std::cout << std::endl << "Convex Hull is formed by following points:" << std::endl;
		for (sf::Vertex v : convexHull) {
			std::cout << "( " << v.position.x << " | " << v.position.y << " )" << std::endl;
		}

		std::cout << std::endl;
		break;

	case eModeGraphic:
		sf::ContextSettings settings;
		settings.antialiasingLevel = 8;
		sf::RenderWindow window(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), "Quickhull", sf::Style::Default, settings);

		convexHull = convex_hull::quickHull(points);
		update(window, points);

		while (window.isOpen())
		{
			sf::Event event;
			while (window.pollEvent(event))
			{
				if (event.type == sf::Event::Closed)
					window.close();

				if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Key::Right) {
					step++;
					update(window, points);
				}
				if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Key::Left) {
					step--;
					update(window, points);
				}
			}

			window.display();
		}
		break;
	}

	return 0;
}