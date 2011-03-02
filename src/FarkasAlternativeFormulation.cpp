/*
 * FarkasAlternativeFormulation.cpp
 *
 *  Created on: Feb 23, 2011
 *      Author: hatefi
 */

#include "../inc/FarkasAlternativeFormulation.h"

FarkasAlternativeFormulation::FarkasAlternativeFormulation(Network *networkModel = NULL, const char * outputStream = NULL)
	: CoreFormulation(networkModel, outputStream){
	if( networkModel != NULL ) {
		yices_expr null = (yices_expr)NULL;
		mConservationLawVariables = vector<yices_expr>(mNetworkModel -> getNodeNumber(), null);
		mFlowLowerBoundVariables = vector<yices_expr>(mNetworkModel -> getEdgeNumber(), null);
		mFlowUpperBoundVariables = vector<yices_expr>(mNetworkModel -> getEdgeNumber(), null);
		mDemandUpperBoundVariables = vector<yices_expr>(mNetworkModel -> getDemandNumber(), null);
		mEdgeSelectionVariables = vector<yices_expr>(mNetworkModel -> getEdgeNumber(), null);
	}
	else {
		mConservationLawVariables = vector<yices_expr>();
		mFlowLowerBoundVariables = vector<yices_expr>();
		mFlowUpperBoundVariables = vector<yices_expr>();
		mDemandUpperBoundVariables = vector<yices_expr>();
		mEdgeSelectionVariables = vector<yices_expr>();
	}
}

/*
 * 1- Create logical context
 * 2- Create variable and function declaration
 * 3- Create and store variable and function expression e.g. (fun n) ...
 */
void FarkasAlternativeFormulation::initialize() {

	CoreFormulation::initialize();

	if( !isLogicalContextValid() ) {
		cerr<<"Error, Invalid logical context.\n";
		return;
	}

	/*
	 * Function declarations
	 */

    yices_var_decl conservationFunctionDeclaration = yices_mk_var_decl(mLogicalContext, (char *)"z0", mYicesTypeFunctionInt2Real);
    yices_var_decl flowLowerBoundFunctionDeclaration = yices_mk_var_decl(mLogicalContext, (char *)"z1", mYicesTypeFunctionInt2Real);
    yices_var_decl flowUpperBoundFunctionDeclaration = yices_mk_var_decl(mLogicalContext, (char *)"z2", mYicesTypeFunctionInt2Real);
    yices_var_decl demandUpperBoundFunctionDeclaration = yices_mk_var_decl(mLogicalContext, (char *)"z3", mYicesTypeFunctionInt2Real);
    yices_var_decl edgeSelectionFunctionDeclaration = yices_mk_var_decl(mLogicalContext, (char *)"x", mYicesTypeFunctionInt2Bool);

    /*
     * Variable declaration
     */
    yices_var_decl adequacyLevelVariableDeclaration = yices_mk_var_decl(mLogicalContext, (char *)"z", mYicesTypeReal);


    /*
     * Creating yices_expr from function declarations
     */
    yices_expr conservationFunction = yices_mk_var_from_decl(mLogicalContext, conservationFunctionDeclaration);
    yices_expr flowLowerBoundFunction = yices_mk_var_from_decl(mLogicalContext, flowLowerBoundFunctionDeclaration);
    yices_expr flowUpperBoundFunction = yices_mk_var_from_decl(mLogicalContext, flowUpperBoundFunctionDeclaration);
    yices_expr demandUpperBoundFunction = yices_mk_var_from_decl(mLogicalContext, demandUpperBoundFunctionDeclaration);
    yices_expr edgeSelectionFunction = yices_mk_var_from_decl(mLogicalContext, edgeSelectionFunctionDeclaration);

    /*
     * Creating yices_expr from variable declarations
     */
    mAdequacyLevelVariable = yices_mk_var_from_decl(mLogicalContext, adequacyLevelVariableDeclaration);

    /*
     * Creating an array corresponding to conservation law constraints
     */
	for(int i = 0; i < mNetworkModel -> getNodeNumber(); i++) {
		yices_expr yicesExpressionOfNodeID = yices_mk_num(mLogicalContext, (mNetworkModel -> getNodeFromIndex(i)).getNodeID());
		mConservationLawVariables[i] = yices_mk_app(mLogicalContext, conservationFunction, &yicesExpressionOfNodeID, 1);
	}

    /*
     * Creating arrays corresponding to lower and upper bound constraints, and also edge selection variable (x)
     */
	for(int i = 0; i < mNetworkModel -> getEdgeNumber(); i++) {
		yices_expr yicesExpressionOfEdgeIndex = yices_mk_num(mLogicalContext, i);
		mFlowLowerBoundVariables[i] = yices_mk_app(mLogicalContext, flowLowerBoundFunction, &yicesExpressionOfEdgeIndex, 1);
		mFlowUpperBoundVariables[i] = yices_mk_app(mLogicalContext, flowUpperBoundFunction, &yicesExpressionOfEdgeIndex, 1);
		mEdgeSelectionVariables[i] = yices_mk_app(mLogicalContext, edgeSelectionFunction, &yicesExpressionOfEdgeIndex, 1);
	}

    /*
     * Creating an array corresponding to demand upper bound constraints
     */
	for(int i = 0; i < mNetworkModel -> getDemandNumber(); i++) {
		yices_expr yicesExpressionOfNodeID = yices_mk_num(mLogicalContext, (mNetworkModel -> getNodeFromDemandIndex(i)).getNodeID());
		mDemandUpperBoundVariables[i] = yices_mk_app(mLogicalContext, demandUpperBoundFunction, &yicesExpressionOfNodeID, 1);
	}

}

void FarkasAlternativeFormulation::createLogicalContext() {

	/*
	 * Initialize the logical context.
	 * Adding constraints related to flow and demand variables (f and D)
	 * Adding other constraints that are related to generator power and
	 * vector b depends on the problem.
	 */

//	initialize();

	/*
	 * Constraints corresponding to flows: Transpose(N)z0 + z1 + z2 = 0
	 */
	yices_expr argumentList[3];
	for(int i = 0; i < mNetworkModel -> getEdgeNumber(); i++) {
		const Edge &currentEdge = mNetworkModel -> getEdgeFromIndex(i);
		int fromNodeIndex = currentEdge.getFromNodeIndex();
		int toNodeIndex = currentEdge.getToNodeIndex();

		yices_expr minusArgs[2] = {mConservationLawVariables[fromNodeIndex], mConservationLawVariables[toNodeIndex]};
		argumentList[0] = yices_mk_sub(mLogicalContext, minusArgs, 2);
		argumentList[1] = mFlowLowerBoundVariables[i];
		argumentList[2] = mFlowUpperBoundVariables[i];

		yices_expr sum = yices_mk_sum(mLogicalContext, argumentList, 3);
		yices_expr orArgs[2];

		orArgs[0] = yices_mk_eq(mLogicalContext, sum, getZEROExpression());
		orArgs[1] = yices_mk_not(mLogicalContext, mEdgeSelectionVariables[i]);

		yices_expr constraint_i = yices_mk_or(mLogicalContext, orArgs, 2);

		yices_assert(mLogicalContext, constraint_i); // Submit the constraint to the logical context
	}

	/*
	 * Constraints corresponding to demand variables: Transpose(L)z0 + z3 + z >= 0
	 */
	for(int i = 0; i < mNetworkModel -> getDemandNumber(); i++) {
		int nodeIndex = mNetworkModel -> getNodeIndexFromDemandIndex(i);

		argumentList[0] = mConservationLawVariables[nodeIndex];
		argumentList[1] = mDemandUpperBoundVariables[i];
		argumentList[2] = mAdequacyLevelVariable;

		yices_expr sum = yices_mk_sum(mLogicalContext, argumentList, 3);
		yices_expr constraint_i = yices_mk_ge(mLogicalContext, sum, getZEROExpression());

		yices_assert(mLogicalContext, constraint_i); // Submit the constraint to the logical context
	}

	/*
	 * z1 <= 0 and z2 >= 0
	 */
	for(int i = 0; i < mNetworkModel -> getEdgeNumber(); i++) {
		argumentList[0] = yices_mk_le(mLogicalContext, mFlowLowerBoundVariables[i], getZEROExpression());
		argumentList[1] = yices_mk_ge(mLogicalContext, mFlowUpperBoundVariables[i], getZEROExpression());

		yices_expr constraint_i = yices_mk_and(mLogicalContext, argumentList, 2);

		yices_assert(mLogicalContext, constraint_i); // Submit the constraint to the logical context
	}

	/*
	 * z <= 0
	 */
	argumentList[0] = yices_mk_le(mLogicalContext, mAdequacyLevelVariable, getZEROExpression());
	yices_assert(mLogicalContext, argumentList[0]); // Submit the constraint to the logical context

	/*
	 * z3 >= 0
	 */
	for(int i = 0; i < mNetworkModel -> getDemandNumber(); i++) {

		yices_expr constraint_i = yices_mk_ge(mLogicalContext, mDemandUpperBoundVariables[i], getZEROExpression());

		yices_assert(mLogicalContext, constraint_i); // Submit the constraint to the logical context
	}

}


FarkasAlternativeFormulation::~FarkasAlternativeFormulation() {
}
