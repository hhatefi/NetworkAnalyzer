/*
 * Node.cpp
 *
 *  Created on: Feb 22, 2011
 *      Author: hatefi
 */

#include "../inc/Node.h"

Node::Node(int nodeID = -1, NodeType nodeType = NORMAL, int outgoingEdgeNumber = 0, int incomingEdgeNumber = 0) :
	outgoingEdgeIndices(), incomingEdgeIndices() {
	setNodeID(nodeID);
	setNodeType(nodeType);
	outgoingEdgeIndices.reserve(outgoingEdgeNumber);
	incomingEdgeIndices.reserve(incomingEdgeNumber);
}

Node::~Node() {
}
