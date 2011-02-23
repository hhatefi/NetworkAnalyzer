/*
 * Demand.h
 *
 *  Created on: Feb 22, 2011
 *      Author: hatefi
 */

#ifndef DEMAND_H_
#define DEMAND_H_

#include "../inc/Node.h"

class Demand: public Node {
private:
	double nominalDemand;
public:
	Demand(int, NodeType, int, int, double);

	/*
	 * Accessors
	 */
	double getNominalDemand() { return nominalDemand; }

	/*
	 * Mutators
	 */
	void setNominalDemand(double nominalDemand) { this -> nominalDemand = nominalDemand; }

	virtual ~Demand();
};

#endif /* DEMAND_H_ */
