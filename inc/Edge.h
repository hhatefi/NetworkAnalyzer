/*
 * Edge.h
 *
 *  Created on: Feb 22, 2011
 *      Author: hatefi
 */

#ifndef EDGE_H_
#define EDGE_H_

class Edge {
private:
	int fromNodeIndex, toNodeIndex;
	double capacity, resistance;
public:
	Edge();
	Edge(int, int, double, double);
	/*
	 * Accessors
	 */
	int getFromNodeIndex() const 	{ return fromNodeIndex; }
	int getToNodeIndex() const 		{ return toNodeIndex; }
	double getCapacity() const 		{ return capacity; }
	double getResistance() const 	{ return resistance; }

	/*
	 * Mutators
	 */
	void setEdge(int, int, double, double);
	void setFromNodeIndex(int fromNodeIndex) { this -> fromNodeIndex = fromNodeIndex; }
	void setToNodeIndex(int toNodeIndex) 	{ this -> toNodeIndex = toNodeIndex; }
	void setCapacity(double capacity) 		{ this -> capacity = capacity; }
	void setResistance(double resistance) 	{ this -> resistance = resistance; }

	virtual ~Edge();
};

#endif /* EDGE_H_ */
