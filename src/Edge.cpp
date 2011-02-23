/*
 * Edge.cpp
 *
 *  Created on: Feb 22, 2011
 *      Author: hatefi
 */

#include "../inc/Edge.h"

// Initialize to default values
Edge::Edge() {
	this -> setEdge( -1 /* fromNodeIndex */, -1 /* toNodeIndex */, 0 /* capacity */, 0 /* resistance */);
}

Edge::Edge(int fromNodeIndex, int toNodeIndex, double capacity, double resistance) {
	this -> setEdge(fromNodeIndex, toNodeIndex, capacity, resistance);
}

void Edge::setEdge(int fromNodeIndex, int toNodeIndex, double capacity, double resistance) {
	this -> setFromNodeIndex(fromNodeIndex);
	this -> setToNodeIndex(toNodeIndex);
	this -> setCapacity(capacity);
	this -> setResistance(resistance);
}

Edge::~Edge() {
}
