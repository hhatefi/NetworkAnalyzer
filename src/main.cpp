//============================================================================
// Name        : NerworkAnalyzer.cpp
// Author      : Hassan Hatefi
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
using namespace std;

#include "../inc/NetworkAnalyzer.h"
#include "../inc/FixedConfigurationMinCardinalityAttack.h"

int main(int argc, char**argv) {

	int thresholdArgIndex = -1, modelFileArgIndex = -1, outputFileArgIndex = -1, mode = 1;
	for(int i = 1; i < argc; i++) {
		if(!strncmp("-t", argv[i], 3)) {
			thresholdArgIndex = ++i;
		}
		else if (!strncmp("-m", argv[i], 3)) {
			modelFileArgIndex = ++i;
		}
		else if (!strncmp("-f", argv[i], 3)) {
			mode = 0;
		}
		else if (!strncmp("-o", argv[i], 3)) {
			outputFileArgIndex = ++i;
		}
	}
	if(thresholdArgIndex == -1 || modelFileArgIndex == -1) {
		cout<<"Error using Network analyzer. Correct way of using is:\nNetworkAnalyzer -t THRESHOLD -m MODEL\n";
	}
	else {
		if( mode == 0 ) {
			Network n;
			n.readNetworkStructureFromFile(argv[modelFileArgIndex]);
			if(!n.isValid()) cerr<<"Error, invalid network.\n";

			const char *outputFileName = NULL;
			if ( outputFileArgIndex != -1 )
				outputFileName = argv[outputFileArgIndex];

			FixedConfigurationMinCardinalityAttack f(&n, outputFileName);
			f.setThreshold(atof(argv[thresholdArgIndex]));
			//int lst = 1;
			//f.setInactiveGenerators(&lst, 1);
			f.createLogicalContext();
			f.check();
		}
		else {
			const char *outputFileName = NULL;
			if ( outputFileArgIndex != -1 )
				outputFileName = argv[outputFileArgIndex];
			doAnalysis(argv[modelFileArgIndex], atof(argv[thresholdArgIndex]), outputFileName);

		}
	}

	return 0;
}
