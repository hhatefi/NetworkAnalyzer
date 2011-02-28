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
	mNodeAddressList(), mEdgeList(), mGeneratorIndexList(), mDemandIndexList(), mNodeIDNodeIndexMap() {
	setNodeNumber(0);
	setEdgeNumber(0);
	setGeneratorNumber(0);
	setDemandNumber(0);

	setNetworkInvalid();
}

void Network::addEdge(int fromNode, int toNode, double capacity, double resistance) {
	Edge e(fromNode, toNode, capacity, resistance);
	mEdgeList.push_back(e);
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

	networkDescriptionFile>>mNodeNumber>>mEdgeNumber>>mGeneratorNumber>>mDemandNumber;
	if( mNodeNumber < 0 || mEdgeNumber < 0 || mGeneratorNumber < 0 || mDemandNumber < 0) {
		cerr<<"ERROR: Number of nodes, edges, generators and demand must be nonnegative.\n";
		setNetworkInvalid();
		networkDescriptionFile.close();
		return;
	}

	/* Allocation part */
	mNodeAddressList.resize(mNodeNumber, NULL);
	mEdgeList.reserve(mEdgeNumber);
	mGeneratorIndexList.reserve(mGeneratorNumber);
	mDemandIndexList.reserve(mDemandNumber);
//	nodeIDDictionary.reserve(nodeNumber);

	/*
	 * Read edges from file
	 */
	vector<int> outgoingEdgeCounter(mNodeNumber, 0), incomingEdgeCounter(mNodeNumber, 0), nodeIDList(mNodeNumber, 0);
	int nodeCounter = 0;
	map<int,int>::iterator nodeIDNodeIndexIterator;
	for (int i = 0; i < mEdgeNumber; i++) {
		int fromNodeID, toNodeID;
		double capacity, resistance;

		/*
		 * Read fromNodeID, toNodeID, capacity and resistance of the current node
		 */
		networkDescriptionFile>>fromNodeID>>toNodeID>>resistance>>capacity;
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
		nodeIDNodeIndexIterator = mNodeIDNodeIndexMap.find(fromNodeID);
		if( nodeIDNodeIndexIterator == mNodeIDNodeIndexMap.end() ) {
			fromNodeIndex = nodeCounter++;
			mNodeIDNodeIndexMap.insert(pair<int,int>(fromNodeID, fromNodeIndex));
			nodeIDList[fromNodeIndex] = fromNodeID;
		}
		else {
			fromNodeIndex = (*nodeIDNodeIndexIterator).second;
		}
		outgoingEdgeCounter[fromNodeIndex]++;

		/*
		 * Find/introduce the index of toNodeID
		 */
		nodeIDNodeIndexIterator = mNodeIDNodeIndexMap.find(toNodeID);
		if( nodeIDNodeIndexIterator == mNodeIDNodeIndexMap.end() ) {
			toNodeIndex = nodeCounter++;
			mNodeIDNodeIndexMap.insert(pair<int,int>(toNodeID, toNodeIndex));
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
	for (int i = 0; i < mGeneratorNumber; i++) {
		int nodeID;
		double minPower, maxPower;

		networkDescriptionFile>>nodeID>>minPower>>maxPower;
		if (minPower < 0 || maxPower < 0) {
			cerr<<"Minimum and maximum power of each generator must be nonnegative.\n";
			setNetworkInvalid();
			networkDescriptionFile.close();
			return;
		}

		int nodeIndex = mNodeIDNodeIndexMap[nodeID];
		mGeneratorIndexList.push_back(nodeIndex);
		mNodeAddressList[nodeIndex] = new Generator(nodeID, GENERATOR, outgoingEdgeCounter[nodeIndex], incomingEdgeCounter[nodeIndex], minPower, maxPower, mGeneratorIndexList.size() - 1);
	}

	/*
	 * Read dummy "Load <Number>"
	 */
	networkDescriptionFile>>dummyStr>>dummyInt;

	/*
	 * Read Loads
	 */
	for (int i = 0; i < mDemandNumber; i++) {
		int nodeID;
		double nominalPower;
		networkDescriptionFile>>nodeID>>nominalPower;
		if (nominalPower < 0) {
			cerr<<"Nominal demand of each load must be nonnegative.\n";
			setNetworkInvalid();
			networkDescriptionFile.close();
			return;
		}

		int nodeIndex = mNodeIDNodeIndexMap[nodeID];
		mDemandIndexList.push_back(nodeIndex);
		mNodeAddressList[nodeIndex] = new Demand(nodeID, DEMAND, outgoingEdgeCounter[nodeIndex], incomingEdgeCounter[nodeIndex], nominalPower, mDemandIndexList.size() - 1);
	}

	/*
	 * Allocate normal nodes (apart from generators and demands)
	 */
	for(int i = 0; i < mNodeNumber; i++) {
		if( mNodeAddressList[i] == NULL ) {
			mNodeAddressList[i] = new Node(nodeIDList[i], NORMAL, outgoingEdgeCounter[i], incomingEdgeCounter[i]);
		}
	}

	/*
	 * Fill out the incoming and outgoing part of nodes
	 */

	for(int i = 0; i < mEdgeNumber; i++) {
		mNodeAddressList[mEdgeList[i].getFromNodeIndex()] -> addOutgoingEdge(i);
		mNodeAddressList[mEdgeList[i].getToNodeIndex()] -> addIncomingEdge(i);
	}

	setNetworkValid();
	networkDescriptionFile.close();

}

Network::~Network() {
	for(int i =0; i < mNodeNumber; i++) {
		delete mNodeAddressList[i];
	}
}
