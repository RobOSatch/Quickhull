#pragma once
#include <iostream>
#include <vector>
#include <stdlib.h>
#include <time.h>

#include <glm/glm.hpp>
#include <glm/gtx/closest_point.hpp>

#include <chrono>

#include <string>
#include <fstream>
#include <algorithm>

enum MODE {
	eModePerformance,
	eModeGraphic
};

namespace convex_hull {

	MODE mode = eModeGraphic;
	std::vector<std::vector<sf::Vertex>> hullHistory;

}

namespace {

	using namespace convex_hull;
	
	sf::Vertex mMin;

	// Returns a negative number if c lies left of ab
	// Returns a positive number if c lies right of ab
	// Returns 0 if c is colinear with ab
	int orient2D(sf::Vertex a, sf::Vertex b, sf::Vertex c)
	{
		return (a.position.x - c.position.x) * (b.position.y - c.position.y) - (a.position.y - c.position.y) * (b.position.x - c.position.x);
	}

	// Returns the distance of a query point to the line ab
	float distance(sf::Vertex a, sf::Vertex b, sf::Vertex query)
	{
		return abs((query.position.y - a.position.y) * (b.position.x - a.position.x) - (b.position.y - a.position.y) * (query.position.x - a.position.x));
	}

	// Finds the point in set, which is farthest from pq and adds it to the convex hull. After that the set is partitioned.
	void findHull(std::vector<sf::Vertex>& set, sf::Vertex p, sf::Vertex q, std::vector<sf::Vertex>& convexHull)
	{
		if (set.size() == 0) return;

		int farthest = 0;
		for (int i = 1; i < set.size(); i++) {
			if (distance(p, q, set[i]) > distance(p, q, set[farthest])) farthest = i;
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

		if (mode == eModeGraphic) {
			std::vector<sf::Vertex> copy = convexHull;
			copy.push_back(mMin);
			convex_hull::hullHistory.push_back(copy);
		}

		findHull(subset1, p, farthestVertex, convexHull);
		findHull(subset2, farthestVertex, q, convexHull);
	}
}

namespace convex_hull {

	// Finds the min and max points in x-direction and adds them to the convex hull. Then partitions the remaining points into points that are
	// left of the minmax-line and points that are right of the minmax-line. 
	std::vector<sf::Vertex> quickHull(std::vector<sf::Vertex> points)
	{
		std::vector<sf::Vertex> convexHull;
		int minIndex = 0;
		int maxIndex = 0;

		if (mode == eModeGraphic) hullHistory.push_back(convexHull);

		// Find min and max
		for (int i = 1; i < points.size(); i++) {
			if (points[i].position.x < points[minIndex].position.x) minIndex = i;
			if (points[i].position.x > points[maxIndex].position.x) maxIndex = i;
		}

		convexHull.push_back(points[minIndex]);
		convexHull.push_back(points[maxIndex]);
		points.erase(points.begin() + minIndex);
		points.erase(points.begin() + maxIndex);

		std::vector<sf::Vertex> rightSet, leftSet;

		// Partition into two sets
		for (int i = 0; i < points.size(); i++) {
			int det = orient2D(convexHull[0], convexHull[1], points[i]);

			if (det > 0) rightSet.push_back(sf::Vertex(points[i].position, sf::Color::Magenta));
			else if (det < 0) leftSet.push_back(sf::Vertex(points[i].position, sf::Color::Magenta));
		}

		sf::Vertex min = convexHull[0];
		sf::Vertex max = convexHull[1];
		
		if (mode == eModeGraphic) {
			mMin = min;
			std::vector<sf::Vertex> copy = convexHull;
			copy.push_back(mMin);
			hullHistory.push_back(copy);
		}

		findHull(leftSet, min, max, convexHull);
		findHull(rightSet, max, min, convexHull);

		return convexHull;
	}

}