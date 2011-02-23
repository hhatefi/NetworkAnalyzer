/*
 * Generator.h
 *
 *  Created on: Feb 22, 2011
 *      Author: hatefi
 */

#ifndef GENERATOR_H_
#define GENERATOR_H_

#include "../inc/Node.h"

class Generator: public Node {
private:
	double minPower, maxPower;
public:
	Generator(int, NodeType, int, int, double, double);

	/*
	 * Accessors
	 */
	double getMinPower() { return minPower; }
	double getMaxPower() { return maxPower; }

	/*
	 * Mutators
	 */
	void setMinPower(double minPower) { this -> minPower = minPower; }
	void setMaxPower(double maxPower) { this -> maxPower = maxPower; }

	virtual ~Generator();
};

#endif /* GENERATOR_H_ */
