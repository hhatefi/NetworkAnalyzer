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
	int mDemandIndex;
public:
	Demand(int, NodeType, int, int, double, int);

	/*
	 * Accessors
	 */
	double getNominalDemand() const{ return nominalDemand; }
	int getDemandIndex() const { return mDemandIndex; }

	/*
	 * Mutators
	 */
	void setNominalDemand(double nominalDemand) { this -> nominalDemand = nominalDemand; }
	void setDemandIndex(int demandIndex) { mDemandIndex = demandIndex; }

	virtual ~Demand();
};

#endif /* DEMAND_H_ */
