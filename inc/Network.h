/*
 * Network.h
 *
 *  Created on: Feb 22, 2011
 *      Author: hatefi
 */

#ifndef NETWORK_H_
#define NETWORK_H_

#include "../inc/Edge.h"
#include "../inc/Node.h"
#include "../inc/Generator.h"
#include "../inc/Demand.h"

#include <map>

class Network {
private:
	int mNodeNumber, mEdgeNumber, mGeneratorNumber, mDemandNumber;

	vector<Node *> mNodeAddressList;
	vector<Edge> mEdgeList;
	vector<int> mGeneratorIndexList, mDemandIndexList;
	map<int, int> mNodeIDNodeIndexMap;

	bool mIsValid;

	/*
	 * Mutators
	 */
	void setNodeNumber(int nodeNumber) {
		this -> mNodeNumber = nodeNumber;
	}
	void setEdgeNumber(int edgeNumber) {
		this -> mEdgeNumber = edgeNumber;
	}
	void setGeneratorNumber(int generatorNumber) {
		this -> mGeneratorNumber = generatorNumber;
	}
	void setDemandNumber(int demandNumber) {
		this -> mDemandNumber = demandNumber;
	}
	void setNetworkInvalid() {
		this -> mIsValid = false;
	}
	void setNetworkValid() {
		this -> mIsValid = true;
	}

	/*
	 * Add an edge to the network
	 */
	void addEdge(int, int, double, double);
public:
	Network();

	/*
	 * Accessors
	 */
	int getNodeNumber() const {
		return mNodeNumber;
	}
	int getEdgeNumber() const {
		return mEdgeNumber;
	}
	int getGeneratorNumber() const {
		return mGeneratorNumber;
	}
	int getDemandNumber() const {
		return mDemandNumber;
	}
	bool isValid() const {
		return mIsValid;
	}

	const Node& getNodeFromIndex(int index) const {
		return (*mNodeAddressList[index]);
	}
	const Node& getNodeFromNodeID(int nodeID) const {
		return (*mNodeAddressList[(*mNodeIDNodeIndexMap.find(nodeID)).second]);
	}
	const Node& getNodeFromDemandIndex(int demandIndex) const {
		return getNodeFromIndex(mDemandIndexList[demandIndex]);
	}
	int getNodeIndexFromDemandIndex(int demandIndex) const {
		return mDemandIndexList[demandIndex];
	}
	const Node& getNodeFromGeneratorIndex(int generatorIndex) const {
		return getNodeFromIndex(mGeneratorIndexList[generatorIndex]);
	}
	int getNodeIndexFromGeneratorIndex(int generatorIndex) const {
		return mGeneratorIndexList[generatorIndex];
	}
	const Edge& getEdgeFromIndex(int index) const {
		return mEdgeList[index];
	}

	void readNetworkStructureFromFile(const char *);
	virtual ~Network();
};

#endif /* NETWORK_H_ */
