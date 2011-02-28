/*
 * FarkasAlternativeFormulation.h
 *
 *  Created on: Feb 23, 2011
 *      Author: hatefi
 */

#ifndef FARKASALTERNATIVEFORMULATION_H_
#define FARKASALTERNATIVEFORMULATION_H_

#include "../inc/CoreFormulation.h"

class FarkasAlternativeFormulation: public CoreFormulation {
protected:
	vector<yices_expr> mConservationLawVariables, mFlowLowerBoundVariables, mFlowUpperBoundVariables, mDemandUpperBoundVariables, mEdgeSelectionVariables;
	yices_expr mAdequacyLevelVariable;
	virtual void initialize();
public:

	FarkasAlternativeFormulation(Network *, const char *);
	virtual void createLogicalContext();

	virtual ~FarkasAlternativeFormulation();
};

#endif /* FARKASALTERNATIVEFORMULATION_H_ */
