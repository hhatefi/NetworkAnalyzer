/*
 * Node.h
 *
 *  Created on: Feb 22, 2011
 *      Author: hatefi
 */

#ifndef NODE_H_
#define NODE_H_

#include <vector>
using namespace std;

class Network;

enum NodeType {NORMAL, GENERATOR, DEMAND };

class Node {
private:
	int nodeID;
	NodeType nodeType;
	vector<int> outgoingEdgeIndices, incomingEdgeIndices;

	void addOutgoingEdge(int edgeIndex) { outgoingEdgeIndices.push_back(edgeIndex); }
	void addIncomingEdge(int edgeIndex) { incomingEdgeIndices.push_back(edgeIndex); }
public:
	Node(int, NodeType, int, int);

	/*
	 * Accessors
	 */
	int getNodeID() 		{ return nodeID; }
	int getNodeType() 	{ return nodeType; }

	int getOutgoingEdgeIndex(int i) { return outgoingEdgeIndices[i]; }
	int getIncomingEdgeIndex(int i) { return incomingEdgeIndices[i]; }

	/*
	 * Mutators
	 */
	void setNodeID(int nodeID) { this -> nodeID = nodeID; }
	void setNodeType(NodeType nodeType) { this -> nodeType = nodeType; }

	void setNumberOfOutgoingEdges(int size) { this -> outgoingEdgeIndices.reserve(size); }
	void setNumberOfIncomingEdges(int size) { this -> incomingEdgeIndices.reserve(size); }
	virtual ~Node();
	friend class Network;
};

#endif /* NODE_H_ */
