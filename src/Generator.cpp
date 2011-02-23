/*
 * Generator.cpp
 *
 *  Created on: Feb 22, 2011
 *      Author: hatefi
 */

#include "../inc/Generator.h"

Generator::Generator(int nodeID = -1, NodeType nodeType = GENERATOR,
					 int outgoingEdgeNumber = 0, int incomingEdgeNumber = 0,
					 double minPower = 0.0, double maxPower = 0.0 ) : Node(nodeID, nodeType, outgoingEdgeNumber, incomingEdgeNumber) {
	setMinPower(minPower);
	setMaxPower(maxPower);
}

Generator::~Generator() {
}
