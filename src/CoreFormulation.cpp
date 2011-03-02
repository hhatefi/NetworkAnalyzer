/*
 * CoreFormulation.cpp
 *
 *  Created on: Feb 23, 2011
 *      Author: hatefi
 */

#include "../inc/CoreFormulation.h"

CoreFormulation::CoreFormulation(Network *networkModel = NULL, const char *outputFileName = NULL) {
	mNetworkModel = networkModel;
	mLogicalContext = NULL;
	mLogicalContextIsValid = false;
	setLogFileName((char*)outputFileName);

}

void CoreFormulation::initialize() {
	mLogicalContext = yices_mk_context();

	if( mLogicalContext == NULL ) {
		std::cerr<<"Cannot create yices logical context.\n";
		mLogicalContextIsValid = false;
		return;
	}

	mLogicalContextIsValid = true;

	mZERO = yices_mk_num(mLogicalContext,0); // Create a yices expression represents 0
	mYicesTypeInt = yices_mk_type(mLogicalContext, (char *)"int");
    mYicesTypeReal = yices_mk_type(mLogicalContext, (char *)"real");
    mYicesTypeBool = yices_mk_type(mLogicalContext, (char *)"bool");

    mYicesTypeFunctionInt2Real = yices_mk_function_type(mLogicalContext, &mYicesTypeInt, 1, mYicesTypeReal);
    mYicesTypeFunctionInt2Bool = yices_mk_function_type(mLogicalContext, &mYicesTypeInt, 1, mYicesTypeBool);
    mYicesTypeFunctionInt2Int = yices_mk_function_type(mLogicalContext, &mYicesTypeInt, 1, mYicesTypeInt);

}

CoreFormulation::~CoreFormulation() {
	if( mLogicalContext != NULL )
		yices_del_context(mLogicalContext);
}
