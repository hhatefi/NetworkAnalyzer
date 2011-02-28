/*
 * Demand.cpp
 *
 *  Created on: Feb 22, 2011
 *      Author: hatefi
 */

#include "../inc/Demand.h"

Demand::Demand(int nodeID = -1, NodeType nodeType = DEMAND,
		 int outgoingEdgeNumber = 0, int incomingEdgeNumber = 0,
		 double nominalDemand = 0.0, int demandIndex = 0 ) : Node(nodeID, nodeType, outgoingEdgeNumber, incomingEdgeNumber) {
	setNominalDemand(nominalDemand);
	setDemandIndex(demandIndex);
}

Demand::~Demand() {
	// TODO Auto-generated destructor stub
}
