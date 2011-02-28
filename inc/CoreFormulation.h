/*
 * CoreFormulation.h
 *
 *  Created on: Feb 23, 2011
 *      Author: hatefi
 */

#ifndef COREFORMULATION_H_
#define COREFORMULATION_H_

#include "../inc/Network.h"
#include <iostream>
#include <yices_c.h>

#define MAX_FILE_NAME_LENGTH 256

class CoreFormulation {
private:
	bool mLogicalContextIsValid, mEnableOutputStream;
	char mOutputFileName[MAX_FILE_NAME_LENGTH];
	yices_expr mZERO; // 0 as a yices expression
protected:
	Network *mNetworkModel;
	yices_context mLogicalContext;
	yices_type mYicesTypeInt, mYicesTypeReal, mYicesTypeBool;
	yices_type mYicesTypeFunctionInt2Real, mYicesTypeFunctionInt2Bool, mYicesTypeFunctionInt2Int;

	virtual void initialize();

	/*
	 * Accessors
	 */
	const char *getOutputFileName() const { return mOutputFileName; }
	bool isLogicalContextValid() const { return mLogicalContext; }
	bool isOutputStreamEnable() const { return mEnableOutputStream; }

	/*
	 * Mutators
	 */
	void setLogicalContextValidity(bool newValidity) { mLogicalContextIsValid = newValidity; }
	void setArithmeticOnly() { yices_set_arith_only(1); }
	void resetArithmeticOnly() { yices_set_arith_only(0); }
public:
	CoreFormulation(Network *, const char *);

	/*
	 * Accessors
	 */
	Network *getNetworkModel() { return mNetworkModel; }
	const yices_expr getZEROExpression() const { return mZERO; }

	/*
	 * Mutators
	 */
	void setNetworkModel(Network *networkModel) { mNetworkModel = networkModel; }
	void disableOutputStream() { mEnableOutputStream = false; }
	void enableOutputStream() { mEnableOutputStream = true; }
	void setOutputFileName(const char *outputFileName) {
		if ( outputFileName != NULL )
			strncpy(mOutputFileName, outputFileName, MAX_FILE_NAME_LENGTH);
		else
			mOutputFileName[0] = '\0';
		enableOutputStream();
	}

	bool isValid() { return (mNetworkModel != NULL) ? (mNetworkModel -> isValid() && mLogicalContextIsValid) : false; }


	virtual void check() = 0;
	virtual void createLogicalContext() = 0 ;
	virtual ~CoreFormulation();
};

#endif /* COREFORMULATION_H_ */
