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
	int nodeNumber, edgeNumber, generatorNumber, demandNumber;

	vector<Node *> nodeAddressList;
	vector<Edge> edgeList;
	vector<int> generatorIndices, demandIndices;
	map<int, int> nodeIDDictionary;

	bool valid;

	/*
	 * Mutators
	 */
	void setNodeNumber(int nodeNumber)			{ this -> nodeNumber = nodeNumber;}
	void setEdgeNumber(int edgeNumber)			{ this -> edgeNumber = edgeNumber;}
	void setGeneratorNumber(int generatorNumber)	{ this -> generatorNumber = generatorNumber;}
	void setDemandNumber(int demandNumber)		{ this -> demandNumber = demandNumber;}
	void setNetworkInvalid()						{ this -> valid = false; }
	void setNetworkValid()						{ this -> valid = true; }

	/*
	 * Add an edge to the network
	 */
	void addEdge(int, int, double, double);
public:
	Network();

	/*
	 * Accessors
	 */
	int getNodeNumber()			const{ return nodeNumber; }
	int getEdgeNumber()			const{ return edgeNumber; }
	int getGeneratorNumber()		const{ return generatorNumber; }
	int getdemandNumber()		const{ return generatorNumber; }
	bool isValid() 				const{ return valid; }

	void readNetworkStructureFromFile(const char *);
	virtual ~Network();
};

#endif /* NETWORK_H_ */
