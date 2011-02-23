/*
 * Network.cpp
 *
 *  Created on: Feb 22, 2011
 *      Author: hatefi
 */

#include "../inc/Network.h"
#include <iostream>
#include <fstream>
using namespace std;

Network::Network() :
	nodeAddressList(), edgeList(), generatorIndices(), demandIndices(), nodeIDDictionary() {
	setNodeNumber(0);
	setEdgeNumber(0);
	setGeneratorNumber(0);
	setDemandNumber(0);

	setNetworkInvalid();
}

void Network::addEdge(int fromNode, int toNode, double capacity, double resistance) {
	Edge e(fromNode, toNode, capacity, resistance);
	edgeList.push_back(e);
}

void Network::readNetworkStructureFromFile(const char *networkDescriptionFileName) {
	ifstream networkDescriptionFile(networkDescriptionFileName, ifstream::in);

	if (!networkDescriptionFile.good() ) {
		cerr<<"ERROR: Cannot open file "<< networkDescriptionFileName<<endl;
		setNetworkInvalid();
		networkDescriptionFile.close();
		return;
	}


	/*
	 * Read number of nodes, edges, generators and loads.
	 * Then allocate memory for vectors
	 */

	networkDescriptionFile>>nodeNumber>>edgeNumber>>generatorNumber>>demandNumber;
	if( nodeNumber < 0 || edgeNumber < 0 || generatorNumber < 0 || demandNumber < 0) {
		cerr<<"ERROR: Number of nodes, edges, generators and demand must be nonnegative.\n";
		setNetworkInvalid();
		networkDescriptionFile.close();
		return;
	}

	/* Allocation part */
	nodeAddressList.resize(nodeNumber, NULL);
	edgeList.reserve(edgeNumber);
	generatorIndices.reserve(generatorNumber);
	demandIndices.reserve(demandNumber);
//	nodeIDDictionary.reserve(nodeNumber);

	/*
	 * Read edges from file
	 */
	vector<int> outgoingEdgeCounter(nodeNumber, 0), incomingEdgeCounter(nodeNumber, 0), nodeIDList(nodeNumber, 0);
	int nodeCounter = 0;
	map<int,int>::iterator nodeIDNodeIndexIterator;
	for (int i = 0; i < edgeNumber; i++) {
		int fromNodeID, toNodeID;
		double capacity, resistance;

		/*
		 * Read fromNodeID, toNodeID, capacity and resistance of the current node
		 */
		networkDescriptionFile>>fromNodeID>>toNodeID>>capacity>>resistance;
		if( capacity < 0 || resistance < 0) {
			cerr<<"Capacity and resistance of each edge must be nonnegative.\n";
			setNetworkInvalid();
			networkDescriptionFile.close();
			return;
		}

		int fromNodeIndex, toNodeIndex;
		/*
		 * Find/introduce the index of fromNodeID
		 */
		nodeIDNodeIndexIterator = nodeIDDictionary.find(fromNodeID);
		if( nodeIDNodeIndexIterator == nodeIDDictionary.end() ) {
			fromNodeIndex = nodeCounter++;
			nodeIDDictionary.insert(pair<int,int>(fromNodeID, fromNodeIndex));
			nodeIDList[fromNodeIndex] = fromNodeID;
		}
		else {
			fromNodeIndex = (*nodeIDNodeIndexIterator).second;
		}
		outgoingEdgeCounter[fromNodeIndex]++;

		/*
		 * Find/introduce the index of toNodeID
		 */
		nodeIDNodeIndexIterator = nodeIDDictionary.find(toNodeID);
		if( nodeIDNodeIndexIterator == nodeIDDictionary.end() ) {
			toNodeIndex = nodeCounter++;
			nodeIDDictionary.insert(pair<int,int>(toNodeID, toNodeIndex));
			nodeIDList[toNodeIndex] = toNodeID;
		}
		else {
			toNodeIndex = (*nodeIDNodeIndexIterator).second;
		}
		incomingEdgeCounter[toNodeIndex]++;

		addEdge(fromNodeIndex, toNodeIndex, capacity, resistance);
	}


	char dummyStr[100];
	int dummyInt;
	/*
	 * Read dummy "Generator <Number>"
	 */
	networkDescriptionFile>>dummyStr>>dummyInt;

	/*
	 * Read Generators
	 */
	for (int i = 0; i < generatorNumber; i++) {
		int nodeID;
		double minPower, maxPower;

		networkDescriptionFile>>nodeID>>minPower>>maxPower;
		if (minPower < 0 || maxPower < 0) {
			cerr<<"Minimum and maximum power of each generator must be nonnegative.\n";
			setNetworkInvalid();
			networkDescriptionFile.close();
			return;
		}

		int nodeIndex = nodeIDDictionary[nodeID];
		generatorIndices.push_back(nodeIndex);
		nodeAddressList[nodeIndex] = new Generator(nodeID, GENERATOR, outgoingEdgeCounter[nodeIndex], incomingEdgeCounter[nodeIndex], minPower, maxPower);
	}

	/*
	 * Read dummy "Load <Number>"
	 */
	networkDescriptionFile>>dummyStr>>dummyInt;

	/*
	 * Read Loads
	 */
	for (int i = 0; i < demandNumber; i++) {
		int nodeID;
		double nominalPower;
		networkDescriptionFile>>nodeID>>nominalPower;
		if (nominalPower < 0) {
			cerr<<"Nominal demand of each load must be nonnegative.\n";
			setNetworkInvalid();
			networkDescriptionFile.close();
			return;
		}

		int nodeIndex = nodeIDDictionary[nodeID];
		demandIndices.push_back(nodeIndex);
		nodeAddressList[nodeIndex] = new Demand(nodeID, DEMAND, outgoingEdgeCounter[nodeIndex], incomingEdgeCounter[nodeIndex], nominalPower);
	}

	/*
	 * Allocate normal nodes (apart from generators and demands)
	 */
	for(int i = 0; i < nodeNumber; i++) {
		if( nodeAddressList[i] == NULL ) {
			nodeAddressList[i] = new Node(nodeIDList[i], NORMAL, outgoingEdgeCounter[i], incomingEdgeCounter[i]);
		}
	}

	/*
	 * Fill out the incoming and outgoing part of nodes
	 */

	for(int i = 0; i < edgeNumber; i++) {
		nodeAddressList[edgeList[i].getFromNodeIndex()] -> addOutgoingEdge(i);
		nodeAddressList[edgeList[i].getToNodeIndex()] -> addIncomingEdge(i);
	}

	setNetworkValid();
	networkDescriptionFile.close();

}

Network::~Network() {
}
