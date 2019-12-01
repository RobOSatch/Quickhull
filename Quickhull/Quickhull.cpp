// Quickhull.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <vector>
#include <stdlib.h>
#include <time.h>

#include <SFML/Graphics.hpp>
#include <glm/glm.hpp>
#include <glm/gtx/closest_point.hpp>

#include <thread>
#include <chrono>

#include "Timer.h"

int orient2D(sf::Vertex a, sf::Vertex b, sf::Vertex c)
{
	return (a.position.x - c.position.x) * (b.position.y - c.position.y) - (a.position.y - c.position.y) * (b.position.x - c.position.x);
}

float distance(sf::Vertex a, sf::Vertex b, sf::Vertex query, sf::RenderWindow &window)
{
	glm::vec2 glmA = glm::vec2(a.position.x, a.position.y);
	glm::vec2 glmB = glm::vec2(b.position.x, b.position.y);
	glm::vec2 glmQuery = glm::vec2(query.position.x, query.position.y);

	glm::vec2 closestPoint = glm::closestPointOnLine(glmQuery, glmA, glmB);
	
	sf::Vertex line[] = {
		sf::Vertex(sf::Vector2f(closestPoint.x, closestPoint.y)),
		query
	};
	//window.draw(line, 2, sf::Lines);

	return glm::distance(closestPoint, glmQuery);

	//return abs((query.position.y - a.position.y) * (b.position.x - a.position.x) - (b.position.y - a.position.y) * (query.position.x - a.position.x));
}

void findHull(std::vector<sf::Vertex> set, sf::Vertex p, sf::Vertex q, std::vector<sf::Vertex>& convexHull, sf::RenderWindow &window)
{
	if (set.size() == 0) return;

	int farthest = 0;
	for (int i = 1; i < set.size(); i++) {
		if (distance(p, q, set[i], window) > distance(p, q, set[farthest], window)) farthest = i;
	}

	sf::Vertex farthestVertex = set[farthest];
	for (int i = 0; i < convexHull.size(); i++) {
		if (convexHull[i].position == p.position) {
			convexHull.insert(convexHull.begin() + i + 1, farthestVertex);
		}
	}

	std::vector<sf::Vertex> subset1, subset2;
	for (int i = 0; i < set.size(); i++) {
		float detLeft = orient2D(p, farthestVertex, set[i]);
		float detRight = orient2D(farthestVertex, q, set[i]);

		if (!(detLeft > 0 && detRight > 0)) {
			if (detLeft < 0) {
				subset1.push_back(set[i]);
			}
			else if (detRight < 0) {
				subset2.push_back(set[i]);
			}
		}
	}
	set.erase(set.begin() + farthest);
	//window.draw(&convexHull[0], convexHull.size(), sf::LineStrip);
	
	findHull(subset1, p, farthestVertex, convexHull, window);
	findHull(subset2, farthestVertex, q, convexHull, window);
}

std::vector<sf::Vertex> quickHull(std::vector<sf::Vertex> points, sf::RenderWindow &window)
{
	std::vector<sf::Vertex> convexHull;
	int minIndex = 0;
	int maxIndex = 0;

	for (int i = 1; i < points.size(); i++) {
		if (points[i].position.x < points[minIndex].position.x) minIndex = i;
		if (points[i].position.x > points[maxIndex].position.x) maxIndex = i;
	}

	convexHull.push_back(points[minIndex]);
	convexHull.push_back(points[maxIndex]);
	points.erase(points.begin() + minIndex);
	points.erase(points.begin() + maxIndex);

	convexHull[0].color = sf::Color::Magenta;
	convexHull[1].color = sf::Color::Magenta;

	sf::Vertex line[] = {
		convexHull[0],
		convexHull[1]
	};

	std::vector<sf::Vertex> rightSet, leftSet;

	for (int i = 0; i < points.size(); i++) {
		int det = orient2D(convexHull[0], convexHull[1], points[i]);

		if (det > 0) rightSet.push_back(sf::Vertex(points[i].position, sf::Color::Magenta));
		else if (det < 0) leftSet.push_back(sf::Vertex(points[i].position, sf::Color::Magenta));
	}

	for (int i = 0; i < rightSet.size(); i++) {
		sf::CircleShape shape(2.f);
		shape.move(rightSet[i].position);
		window.draw(shape);
	}

	for (int i = 0; i < leftSet.size(); i++) {
		sf::CircleShape shape(2.f);
		shape.move(leftSet[i].position);
		window.draw(shape);
	}
	
	/*window.draw(&leftSet[0], leftSet.size(), sf::Points);
	window.draw(&rightSet[0], rightSet.size(), sf::Points);*/
	
	// Draw line
	//window.draw(line, 2, sf::Lines);

	std::cout << "Convex hull size " << convexHull.size() << std::endl;

	sf::Vertex min = convexHull[0];
	sf::Vertex max = convexHull[1];

	findHull(leftSet, min, max, convexHull, window);
	findHull(rightSet, max, min, convexHull, window);
	convexHull.push_back(min);

	return convexHull;
}

void Update()
{

}

int main()
{	
	int SCREEN_WIDTH = 1920;
	int SCREEN_HEIGHT = 1080;

	srand(time(NULL));
	std::vector<sf::Vertex> points;

	for (int i = 0; i < 500; i++) {
		float x = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (SCREEN_WIDTH * 0.95))) + SCREEN_WIDTH * 0.025;
		float y = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (SCREEN_HEIGHT * 0.95))) + SCREEN_HEIGHT * 0.025;
		points.push_back(sf::Vertex(sf::Vector2f(x, y)));
	}

	sf::ContextSettings settings;
	settings.antialiasingLevel = 8;
	sf::RenderWindow window(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), "Quickhull", sf::Style::Default, settings);

	Timer::start();
	std::vector<sf::Vertex> convexHull = quickHull(points, window);
	Timer::stop();

	window.draw(&convexHull[0], convexHull.size(), sf::LineStrip);

	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();

			if (event.type == sf::Event::KeyPressed)
				Update();
		}

		//window.clear();

		// RENDER
		//window.draw(&points[0], points.size(), sf::Points);

		//window.draw(&convexHull[0], convexHull.size(), sf::LineStrip);

		window.display();
	}

	return 0;
}