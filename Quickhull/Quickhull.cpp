// Quickhull.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <vector>
#include <stdlib.h>
#include <time.h>

#include <SFML/Graphics.hpp>
#include <glm/glm.hpp>
#include <glm/gtx/closest_point.hpp>

int orient2D(sf::Vertex a, sf::Vertex b, sf::Vertex c)
{
	return (a.position.x - c.position.x) * (b.position.y - c.position.y) - (a.position.y - c.position.y) * (b.position.x - c.position.x);
}

float distance(sf::Vertex a, sf::Vertex b, sf::Vertex query, sf::RenderWindow &window)
{
	/*glm::vec2 glmA = glm::vec2(a.position.x, a.position.y);
	glm::vec2 glmB = glm::vec2(b.position.x, b.position.y);
	glm::vec2 glmQuery = glm::vec2(query.position.x, query.position.y);

	glm::vec2 closestPoint = glm::closestPointOnLine(glmQuery, glmA, glmB);
	return glm::distance(closestPoint, glmQuery);*/

	return abs((query.position.y - a.position.y) * (b.position.x - a.position.x) - (b.position.y - a.position.y) * (query.position.x - a.position.x));
}

void findHull(std::vector<sf::Vertex> set, int p, int q, std::vector<sf::Vertex>& convexHull, sf::RenderWindow &window)
{
	if (set.size() == 0) return;

	int farthest = 0;
	for (int i = 1; i < set.size(); i++) {
		if (distance(convexHull[p], convexHull[q], set[i], window) > distance(convexHull[p], convexHull[q], set[farthest], window)) farthest = i;
	}

	convexHull.insert(convexHull.begin() + q, set[farthest]);
	set.erase(set.begin() + farthest);

	std::vector<sf::Vertex> subset1, subset2;
	for (int i = 0; i < set.size(); i++) {
		float detLeft = orient2D(convexHull[p], convexHull[p + 1], set[i]);
		float detRight = orient2D(convexHull[p + 1], convexHull[q], set[i]);

		if (!(detLeft > 0 && detRight > 0)) {
			if (detLeft < 0) {
				subset1.push_back(set[i]);
			}
			else if (detRight < 0) {
				subset2.push_back(set[i]);
			}
		}
	}

	//window.draw(&convexHull[0], convexHull.size(), sf::LineStrip);

	findHull(subset1, p, p + 1, convexHull, window);
	findHull(subset2, p + 1, q, convexHull, window);
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

	sf::Vertex line[] = {
		convexHull[0],
		convexHull[1]
	};

	std::vector<sf::Vertex> leftSet, rightSet;

	for (int i = 0; i < points.size(); i++) {
		int det = orient2D(convexHull[0], convexHull[1], points[i]);

		if (det > 0) leftSet.push_back(sf::Vertex(points[i].position, sf::Color::Green));
		else if (det < 0) rightSet.push_back(sf::Vertex(points[i].position, sf::Color::Red));
	}

	for (int i = 0; i < leftSet.size(); i++) {
		sf::CircleShape shape(2.f);
		shape.move(leftSet[i].position);
		window.draw(shape);
	}

	for (int i = 0; i < rightSet.size(); i++) {
		sf::CircleShape shape(2.f);
		shape.move(rightSet[i].position);
		window.draw(shape);
	}
	
	/*window.draw(&leftSet[0], leftSet.size(), sf::Points);
	window.draw(&rightSet[0], rightSet.size(), sf::Points);*/
	window.draw(line, 2, sf::Lines);

	findHull(rightSet, 0, 1, convexHull, window);
	findHull(leftSet, 1, 0, convexHull, window);

	return convexHull;
}

int main()
{	
	int SCREEN_WIDTH = 1920;
	int SCREEN_HEIGHT = 1080;

	srand(time(NULL));
	std::vector<sf::Vertex> points;

	for (int i = 0; i < 20; i++) {
		float x = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / SCREEN_WIDTH));
		float y = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / SCREEN_HEIGHT));
		points.push_back(sf::Vertex(sf::Vector2f(x, y)));
	}

	sf::ContextSettings settings;
	settings.antialiasingLevel = 8;
	sf::RenderWindow window(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), "Quickhull", sf::Style::Default, settings);

	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
		}

		window.clear();

		// RENDER
		//window.draw(&points[0], points.size(), sf::Points);

		std::vector<sf::Vertex> convexHull = quickHull(points, window);
		convexHull.push_back(convexHull[0]);
		window.draw(&convexHull[0], convexHull.size(), sf::LineStrip);

		window.display();
	}

	return 0;
}