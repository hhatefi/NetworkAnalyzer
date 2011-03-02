/*
 * FixedConfigurationMinCardinalityAttack.cpp
 *
 *  Created on: Feb 24, 2011
 *      Author: hatefi
 */

#include "../inc/FixedConfigurationMinCardinalityAttack.h"

FixedConfigurationMinCardinalityAttack::FixedConfigurationMinCardinalityAttack(
		Network *networkModel, const char *outputStream) :
	FarkasAlternativeFormulation(networkModel, outputStream) {
	if ( networkModel != NULL ) {
		yices_expr null = (yices_expr)NULL;
		mPowerSupplyLowerBoundVariables = vector<yices_expr> (
				mNetworkModel -> getGeneratorNumber(), null);
		mPowerSupplyUpperBoundVariables = vector<yices_expr> (
				mNetworkModel -> getGeneratorNumber(), null);
		mZeroOneVariable = vector<yices_expr> (
				mNetworkModel -> getEdgeNumber(), null);
		mGeneratorAvailabilityList = vector<bool> (
				mNetworkModel -> getGeneratorNumber(), true);
	}
	else {
		mPowerSupplyLowerBoundVariables = vector<yices_expr> ();
		mPowerSupplyUpperBoundVariables = vector<yices_expr> ();
		mZeroOneVariable = vector<yices_expr> ();
		mGeneratorAvailabilityList = vector<bool> ();
	}

	mThreshold = 0.5;

}



/*
 * Return number of active generators
 */

int FixedConfigurationMinCardinalityAttack::getActiveGeneratorNumber() const {
	int sum = 0;
	for(int i = 0; i < mNetworkModel -> getGeneratorNumber(); i++) {
		if( mGeneratorAvailabilityList[i] ) {
			sum++;
		}
	}
	return sum;
}


/*
 * Set the node IDs in inactiveGeneratorNodeIDList array as inactive generators
 * inactiveGeneratorNodeIDList: array of node ID of generators which are inactive
 * inactiveGeneratorNumber: size of inactiveGeneratorNodeIDList array (number of inactive generators)
 */
void FixedConfigurationMinCardinalityAttack::setInactiveGenerators(int *inactiveGeneratorNodeIDList, int inactiveGeneratorNumber){
	/*
	 * Sets all generators active
	 */
	for(int i =0 ; i < mNetworkModel -> getGeneratorNumber(); i++) {
		mGeneratorAvailabilityList[i] = true;
	}
	/*
	 * Set the ones in inactiveGeneratorNodeIDList array inactive
	 */
	for(int i = 0; i < inactiveGeneratorNumber; i++) {
		int generatorIndex = ((Generator&)(mNetworkModel -> getNodeFromNodeID(inactiveGeneratorNodeIDList[i]))).getGeneratorIndex();
		mGeneratorAvailabilityList[generatorIndex] = false;
	}
}

void FixedConfigurationMinCardinalityAttack::initialize() {

	FarkasAlternativeFormulation::initialize();

	if( !isLogicalContextValid() ) {
		return;
	}

	/*
	 * Function declarations
	 */

    yices_var_decl powerGeneratorLowerBoundFunctionDeclaration = yices_mk_var_decl(mLogicalContext, (char *)"z4", mYicesTypeFunctionInt2Real);
    yices_var_decl powerGeneratorUpperBoundFunctionDeclaration = yices_mk_var_decl(mLogicalContext, (char *)"z5", mYicesTypeFunctionInt2Real);
    yices_var_decl zeroOneFunctionDeclaration = yices_mk_var_decl(mLogicalContext, (char *)"xx", mYicesTypeFunctionInt2Int);


    /*
     * Creating yices_expr from function declarations
     */
    yices_expr powerGeneratorLowerBoundFunction = yices_mk_var_from_decl(mLogicalContext, powerGeneratorLowerBoundFunctionDeclaration);
    yices_expr powerGeneratorUpperBoundFunction = yices_mk_var_from_decl(mLogicalContext, powerGeneratorUpperBoundFunctionDeclaration);
    yices_expr zeroOneFunction = yices_mk_var_from_decl(mLogicalContext, zeroOneFunctionDeclaration);

    /*
     * Creating an array corresponding to power generator lower and upper bound constraints
     */
	for(int i = 0; i < mNetworkModel -> getGeneratorNumber(); i++) {
		yices_expr yicesExpressionOfNodeID = yices_mk_num(mLogicalContext, (mNetworkModel -> getNodeFromGeneratorIndex(i)).getNodeID());
		mPowerSupplyLowerBoundVariables[i] = yices_mk_app(mLogicalContext, powerGeneratorLowerBoundFunction, &yicesExpressionOfNodeID, 1);
		mPowerSupplyUpperBoundVariables[i] = yices_mk_app(mLogicalContext, powerGeneratorUpperBoundFunction, &yicesExpressionOfNodeID, 1);
	}

	for(int i = 0; i < mNetworkModel -> getEdgeNumber(); i++) {
		yices_expr num_i = yices_mk_num(mLogicalContext, i);
		mZeroOneVariable[i] = yices_mk_app(mLogicalContext, zeroOneFunction, &num_i , 1);
	}

}



void FixedConfigurationMinCardinalityAttack::createLogicalContext() {

//	yices_set_verbosity(3);

	initialize();

	FarkasAlternativeFormulation::createLogicalContext();

	/*
	 * Adding constraints correspond to power generator variables: Transpose(G)z0 + z4 + z5 >= 0
	 */
	for(int i = 0; i < mNetworkModel -> getGeneratorNumber(); i++) {
		if( mGeneratorAvailabilityList[i] ) {
			yices_expr sumArgs[2];

			sumArgs[0] = mPowerSupplyLowerBoundVariables[i];
			sumArgs[1] = mPowerSupplyUpperBoundVariables[i];

			yices_expr subArgs[2];
			subArgs[0] = yices_mk_sum(mLogicalContext, sumArgs, 2);
			subArgs[1] = mConservationLawVariables[mNetworkModel -> getNodeIndexFromGeneratorIndex(i)];

			yices_expr result = yices_mk_sub(mLogicalContext, subArgs, 2);
			yices_expr constraint_i = yices_mk_ge(mLogicalContext, result, getZEROExpression());

			yices_assert(mLogicalContext, constraint_i);
		}
	}

	/*
	 * Adding constraints correspond to power generator variables: z4 <= 0 and z5 >= 0
	 */
	for(int i = 0; i < mNetworkModel -> getGeneratorNumber(); i++) {
		if( mGeneratorAvailabilityList[i] ) {
			yices_expr andArgs[2];

			andArgs[0] = yices_mk_le(mLogicalContext, mPowerSupplyLowerBoundVariables[i], getZEROExpression());
			andArgs[1] = yices_mk_ge(mLogicalContext, mPowerSupplyUpperBoundVariables[i], getZEROExpression());

			yices_expr constraint_i = yices_mk_and(mLogicalContext, andArgs, 2);

			yices_assert(mLogicalContext, constraint_i);

		}
	}

	/*
	 * Adding constraints that says either x -> (xx == 1) or not(x) -> (xx == 0 )
	 */
	yices_expr *sumXXArgs = new yices_expr[mNetworkModel -> getEdgeNumber()];
	for(int i = 0; i < mNetworkModel -> getEdgeNumber(); i++) {
		yices_expr orArgs[2], andArgs[2];

		andArgs[0] = yices_mk_not(mLogicalContext, mEdgeSelectionVariables[i]);
		andArgs[1] = yices_mk_eq(mLogicalContext, mZeroOneVariable[i], getZEROExpression());

		orArgs[0] = yices_mk_and(mLogicalContext, andArgs, 2);

		andArgs[0] = mEdgeSelectionVariables[i];
		andArgs[1] = yices_mk_eq(mLogicalContext, mZeroOneVariable[i], yices_mk_num(mLogicalContext, 1));

		orArgs[1] = yices_mk_and(mLogicalContext, andArgs, 2);

		yices_expr constraint_i = yices_mk_or(mLogicalContext, orArgs, 2);

		yices_assert(mLogicalContext, constraint_i);
		sumXXArgs[i] = mZeroOneVariable[i];
	}
	yices_expr sumXX = yices_mk_sum(mLogicalContext, sumXXArgs, mNetworkModel -> getEdgeNumber());
	yices_expr consttraintSumXXLessThanY = yices_mk_le(mLogicalContext, sumXX, yices_mk_num(mLogicalContext, mNetworkModel -> getEdgeNumber() - 2) );
	yices_assert(mLogicalContext, consttraintSumXXLessThanY);
	delete [] sumXXArgs;

	/*
	 * Add constraint corresponds to adequacy level constraint
	 */
	yices_expr *arglist = new yices_expr[mNetworkModel -> getEdgeNumber() +
	                                     this -> getActiveGeneratorNumber() +
	                                     mNetworkModel -> getDemandNumber() + 1];

	int index = 0;
	const int BUFFER_SIZE = 256;
	char buffer[BUFFER_SIZE]; // buffer for converting numbers to strings
	/*
	 * Constructing Transpose(u)(z2 - z1)
	 */
	for(int i = 0; i < mNetworkModel -> getEdgeNumber(); i++) {
		yices_expr mulArgs[2], subArgs[2];

		subArgs[0] = mFlowUpperBoundVariables[i];
		subArgs[1] = mFlowLowerBoundVariables[i];

		snprintf(buffer, BUFFER_SIZE, "%f", (mNetworkModel -> getEdgeFromIndex(i)).getCapacity());
		mulArgs[0] = yices_mk_num_from_string(mLogicalContext, buffer);
		mulArgs[1] = yices_mk_sub(mLogicalContext, subArgs, 2);

		arglist[index++] = yices_mk_mul(mLogicalContext, mulArgs, 2);
	}

	/*
	 * Constructing Transpose(Dnom)z3
	 * Finding sum of all nominal demands
	 */
	double sum = 0;
	for(int i = 0; i < mNetworkModel -> getDemandNumber(); i++) {
		yices_expr mulArgs[2];

		double nomDemand = ((Demand&)(mNetworkModel -> getNodeFromDemandIndex(i))).getNominalDemand();
		sum += nomDemand;
		snprintf(buffer, BUFFER_SIZE, "%f", nomDemand);
		mulArgs[0] = yices_mk_num_from_string(mLogicalContext, buffer);
		mulArgs[1] = mDemandUpperBoundVariables[i];

		arglist[index++] = yices_mk_mul(mLogicalContext, mulArgs, 2);
	}

	snprintf(buffer, BUFFER_SIZE, "%f", sum * mThreshold);
	yices_expr args[2] = {
			yices_mk_num_from_string(mLogicalContext, buffer),
			mAdequacyLevelVariable
	};
	arglist[index++] = yices_mk_mul(mLogicalContext, args, 2);

	/*
	 * Constructing Transpose(Pnim)z4 + Transpose(Pmax)z5
	 */
	for(int i = 0; i < mNetworkModel -> getGeneratorNumber(); i++) {
		if( mGeneratorAvailabilityList[i] ) {
			yices_expr mulArgs[2], sumArgs[2];

			snprintf(buffer, BUFFER_SIZE, "%f", ((Generator&)(mNetworkModel -> getNodeFromGeneratorIndex(i))).getMinPower());
			mulArgs[0] = yices_mk_num_from_string(mLogicalContext, buffer);
			mulArgs[1] = mPowerSupplyLowerBoundVariables[i];
			sumArgs[0] = yices_mk_mul(mLogicalContext, mulArgs, 2);

			snprintf(buffer, 256, "%f", ((Generator&)(mNetworkModel -> getNodeFromGeneratorIndex(i))).getMaxPower());
			mulArgs[0] = yices_mk_num_from_string(mLogicalContext, buffer);
			mulArgs[1] = mPowerSupplyUpperBoundVariables[i];
			sumArgs[1] = yices_mk_mul(mLogicalContext, mulArgs, 2);

			arglist[index++] = yices_mk_sum(mLogicalContext, sumArgs, 2);
		}
	}

	yices_expr finalSum = yices_mk_sum(mLogicalContext, arglist, index);
	yices_expr constraint_Ab = yices_mk_lt(mLogicalContext, finalSum, getZEROExpression());

	yices_assert(mLogicalContext, constraint_Ab);

	delete []arglist;

}

void FixedConfigurationMinCardinalityAttack::check() {

	/*
	 * Adding assert+
	 */
	assertion_id *assertionList = new assertion_id[mNetworkModel -> getEdgeNumber()];
	for(int i = 0; i < mNetworkModel -> getEdgeNumber(); i++) {
		int assertID = yices_assert_weighted(mLogicalContext, mEdgeSelectionVariables[i], 1);
		assertionList[i] = assertID;
	}

	/*
	 * Run max sat
	 */
	yices_set_maxsat_initial_cost(2);
	lbool result = yices_max_sat(mLogicalContext);
	if ( result == l_false) {
		cerr<<"Network attack is unsatisfiable.\n";
	}
	else if ( result == l_undef ){
		cerr<<"Network model is incomplete. The result is unknown.\n";
	}

	yices_model model = yices_get_model(mLogicalContext);
	int cost = (int) yices_get_cost(model);
	cerr<<"Cardinality of vulnerable edge set is "<<cost<<", Edge set {";
	for(int i = 0; i < mNetworkModel -> getEdgeNumber(); i++) {
		if( yices_get_assertion_value(model, assertionList[i]) == 0 ) {
			const Edge & e = mNetworkModel -> getEdgeFromIndex(i);
			const Node & fromNode = mNetworkModel -> getNodeFromIndex(e.getFromNodeIndex());
			const Node & toNode = mNetworkModel -> getNodeFromIndex(e.getToNodeIndex());
			cerr<<"\n\t => "<<i<<"( "<< fromNode.getNodeID()<< ", "<<toNode.getNodeID()<<" )";
		}
	}
	cerr<<"\n}\n";

	delete []assertionList;


}

FixedConfigurationMinCardinalityAttack::~FixedConfigurationMinCardinalityAttack() {
}
