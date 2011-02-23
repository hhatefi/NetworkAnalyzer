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
#include "../inc/Network.h"

int main(int argc, char**argv) {

	int thresholdArgIndex = -1, modelFileArgIndex = -1;
	for(int i = 1; i < argc; i++) {
		if(!strncmp("-t", argv[i], 3)) {
			thresholdArgIndex = ++i;
		}
		else if (!strncmp("-m", argv[i], 1000)) {
			modelFileArgIndex = ++i;
		}
	}
	if(thresholdArgIndex == -1 || modelFileArgIndex == -1) {
		cout<<"Error using Network analyzer. Correct way of using is:\nNetworkAnalyzer -t THRESHOLD -m MODEL\n";
	}
	else {
		Network n;
		n.readNetworkStructureFromFile(argv[modelFileArgIndex]);
		if(!n.isValid()) cerr<<"Error, invalid network.\n";
		//doAnalysis(argv[modelFileArgIndex], atof(argv[thresholdArgIndex]));
	}

	return 0;
}
