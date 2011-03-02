/*
 * FixedConfigurationMinCardinalityAttack.h
 *
 *  Created on: Feb 24, 2011
 *      Author: hatefi
 */

#ifndef FIXEDCONFIGURATIONMINCARDINALITYATTACK_H_
#define FIXEDCONFIGURATIONMINCARDINALITYATTACK_H_

#include "../inc/FarkasAlternativeFormulation.h"

class FixedConfigurationMinCardinalityAttack: public FarkasAlternativeFormulation {
private:
	vector<yices_expr> mPowerSupplyLowerBoundVariables, mPowerSupplyUpperBoundVariables;
	vector<yices_expr> mZeroOneVariable;
	vector<bool> mGeneratorAvailabilityList;
	double mThreshold;

protected:
	virtual void initialize();
public:
	/*
	 * Constructors
	 */
	FixedConfigurationMinCardinalityAttack(Network *, const char * = NULL);

	/*
	 * Accessors
	 */
	double getThreshold() const { return mThreshold; }
	int getActiveGeneratorNumber() const;

	/*
	 * Mutators
	 */
	void setThreshold(double threshold) { mThreshold = threshold; }
	void setInactiveGenerators(int *, int);

	virtual void createLogicalContext();
	virtual void check();
	virtual ~FixedConfigurationMinCardinalityAttack();
};

#endif /* FIXEDCONFIGURATIONMINCARDINALITYATTACK_H_ */
