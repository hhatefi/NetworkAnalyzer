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
	int mGeneratorIndex;
public:
	Generator(int, NodeType, int, int, double, double, int);

	/*
	 * Accessors
	 */
	double getMinPower() const{ return minPower; }
	double getMaxPower() const{ return maxPower; }
	int getGeneratorIndex() const { return mGeneratorIndex; }

	/*
	 * Mutators
	 */
	void setMinPower(double minPower) { this -> minPower = minPower; }
	void setMaxPower(double maxPower) { this -> maxPower = maxPower; }
	void setGeneratorIndex(int generatorIndex) { mGeneratorIndex = generatorIndex; }

	virtual ~Generator();
};

#endif /* GENERATOR_H_ */
