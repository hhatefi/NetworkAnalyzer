/*
 * NerworkAnalyzer.h
 *
 *  Created on: Feb 17, 2011
 *      Author: hatefi
 */

#ifndef NERWORKANALYZER_H_
#define NERWORKANALYZER_H_

#define MAX_COMMAND_LENGTH 500
#define MAX_K 5

#include<yices_c.h>
#include <yicesl_c.h>


    enum PowerGridNodeType {
        GENERATOR, LOAD, NORMAL
    };

    struct PowerGridEdge {
        int fromNode, toNode;
        double resistance, capacity;
        yices_expr flow;
        yices_expr flowZeroCond;
    };

    struct PowerGridNode {
        int nOutgoingEdges, nIncomingEdges;
        enum PowerGridNodeType nodeType;
        int genOrLdIndex;

        struct PowerGridEdge **outgoingEdges, **incomingEdges;
    };

    struct PowerGridGenerator {
        int nodeID;
        double minPower, maxPower;
        yices_expr power, select;
    };

    struct PowerGridLoad {
        int nodeID;
        double maxLoad;
        yices_expr power;
    };

    struct PowerGridModel {
    public:
        int nNodes, nEdges, nGenerators, nLoads;

        PowerGridEdge *edges;

        PowerGridNode *nodes;

        PowerGridGenerator *generators;

        PowerGridLoad *loads;

    };

    int findNextSubset(int*, int, int);



    void deleteNodeInPowerGridModel(struct PowerGridModel *pgm) {
        int i;
        for (i = 0; i < pgm -> nNodes; i++) {
            free(pgm -> nodes[i].outgoingEdges);
            free(pgm -> nodes[i].incomingEdges);
        }
    }

    void deletePowerGridModel(struct PowerGridModel *pgm) {
        free(pgm -> nodes);
        free(pgm -> edges);
        free(pgm -> generators);
        free(pgm -> loads);
        free(pgm);
    }

    void fillOutNodes(struct PowerGridModel* pgm) {

        int i;
        for (i = 0; i < pgm -> nNodes; i++) {
            pgm -> nodes[i].nOutgoingEdges = 0;
            pgm -> nodes[i].nIncomingEdges = 0;
            pgm -> nodes[i].nodeType = NORMAL;
        }
        /*
         * Count the number of incoming and outgoing edges;
         */
        for (i = 0; i < pgm -> nEdges; i++) {
            pgm -> nodes[pgm -> edges[i].fromNode].nOutgoingEdges++;
            pgm -> nodes[pgm -> edges[i].toNode].nIncomingEdges++;
        }

        /*
         * Allocate Memory for Incoming and Outgoing edges
         */
        for (i = 0; i < pgm -> nNodes; i++) {
            pgm -> nodes[i].outgoingEdges = (PowerGridEdge **) malloc(pgm -> nodes[i].nOutgoingEdges * sizeof (PowerGridEdge *));
            pgm -> nodes[i].incomingEdges = (PowerGridEdge **) malloc(pgm -> nodes[i].nIncomingEdges * sizeof (PowerGridEdge *));
        }

        /*
         * Set outgoing and incoming edges
         */
        int *iIndex, *oIndex;
        iIndex = (int *) malloc(pgm -> nNodes * sizeof (int));
        oIndex = (int *) malloc(pgm -> nNodes * sizeof (int));

        for (i = 0; i < pgm -> nNodes; i++) {
            iIndex[i] = oIndex[i] = 0;
        }

        for (i = 0; i < pgm -> nEdges; i++) {
            pgm -> nodes[pgm -> edges[i].fromNode].outgoingEdges[oIndex[pgm -> edges[i].fromNode]++] = &(pgm -> edges[i]);
            pgm -> nodes[pgm -> edges[i].toNode].incomingEdges[iIndex[pgm -> edges[i].toNode]++] = &(pgm -> edges[i]);
        }

        free(iIndex);
        free(oIndex);

        /*
         * Set the index of generators
         */
        for (i = 0; i < pgm -> nGenerators; i++) {
            pgm -> nodes[pgm -> generators[i].nodeID].nodeType = GENERATOR;
            pgm -> nodes[pgm -> generators[i].nodeID].genOrLdIndex = i;
        }

        /*
         * Set the index of loads
         */
        for (i = 0; i < pgm -> nLoads; i++) {
            pgm -> nodes[pgm -> loads[i].nodeID].nodeType = LOAD;
            pgm -> nodes[pgm -> loads[i].nodeID].genOrLdIndex = i;
        }

    }

    struct PowerGridModel* readNetworkModel(char *fileName) {
        FILE *model = fopen(fileName, "r");

        if (!model) {
            printf("Cannot open file %s", fileName);
            return NULL;
        }

        struct PowerGridModel *pgm = (struct PowerGridModel*) malloc(sizeof (struct PowerGridModel));

        if (!pgm) {
            printf("cannot allocate memory (for pgm).");
            return NULL;
        }

        /*
         * Read number of nodes, edges, generators and loads.
         * Then allocate memory for pgm
         */

        if (fscanf(model, "%d%d%d%d", &(pgm -> nNodes), &(pgm -> nEdges),
                &(pgm -> nGenerators), &(pgm -> nLoads)) != 4) {
            printf("Input file format is corrupted or incomplet.\n");
            free(pgm);
            fclose(model);
            return NULL;
        }

        /* Allocation part */
        pgm -> edges = (struct PowerGridEdge *) malloc(pgm -> nEdges * sizeof (struct PowerGridEdge));
        pgm -> nodes = (struct PowerGridNode *) malloc(pgm -> nNodes * sizeof (struct PowerGridNode));
        pgm -> generators = (struct PowerGridGenerator *) malloc(pgm -> nGenerators * sizeof (struct PowerGridGenerator));
        pgm -> loads = (struct PowerGridLoad *) malloc(pgm -> nLoads * sizeof (struct PowerGridLoad));


        /*
         * Read Edges
         */
        int i;
        for (i = 0; i < pgm -> nEdges; i++) {
            if (fscanf(model, "%d%d%lf%lf", &(pgm -> edges[i].fromNode), &(pgm -> edges[i].toNode),
                    &(pgm -> edges[i].resistance), &(pgm -> edges[i].capacity)) != 4) {
                printf("Input file format is corrupted or incomplet.\n");
                deletePowerGridModel(pgm);
                fclose(model);
                return NULL;
            }
        }


        char dummyStr[100];
        int dummyInt;

        /*
         * Read Generators
         */
        fscanf(model, "%s%d", dummyStr, &dummyInt);
        for (i = 0; i < pgm -> nGenerators; i++) {
            if (fscanf(model, "%d%lf%lf", &(pgm -> generators[i].nodeID), &(pgm -> generators[i].minPower),
                    &(pgm -> generators[i].maxPower)) != 3) {
                printf("Input file format is corrupted or incomplet.\n");
                deletePowerGridModel(pgm);
                fclose(model);
                return NULL;
            }

        }

        /*
         * Read Loads
         */
        fscanf(model, "%s%d", dummyStr, &dummyInt);
        for (i = 0; i < pgm -> nLoads; i++) {
            if (fscanf(model, "%d%lf", &(pgm -> loads[i].nodeID), &(pgm -> loads[i].maxLoad)) != 2) {
                printf("Input file format is corrupted or incomplet.\n");
                deletePowerGridModel(pgm);
                fclose(model);
                return NULL;
            }
        }

        /*
         * Fill out the incoming and outgoing part of nodes
         */
        fillOutNodes(pgm);


        fclose(model);

        // TODO
        return pgm;



    }

    yices_context createLogicalContext(struct PowerGridModel *pgm, float threshold) {
        yices_context ctx = yices_mk_context();

        /*
         * Definitions
         */
        yices_type typeInt = yices_mk_type(ctx, (char *)"int");
        yices_type typeReal = yices_mk_type(ctx, (char *)"real");
        yices_type typeBool = yices_mk_type(ctx, (char *)"bool");

        yices_type domain2Int[2] = {typeInt, typeInt};
        yices_type domain1Int[1] = {typeInt};

        yices_type typeFlow = yices_mk_function_type(ctx, domain2Int, 2, typeReal);
        yices_type typeGeneratorPower = yices_mk_function_type(ctx, domain1Int, 1, typeReal);
        yices_type typeLoadPower = yices_mk_function_type(ctx, domain1Int, 1, typeReal);
        yices_type typeSelect = yices_mk_function_type(ctx, domain1Int, 1, typeBool);

        yices_var_decl fDecl = yices_mk_var_decl(ctx, (char *)"f", typeFlow);
        yices_var_decl pDecl = yices_mk_var_decl(ctx, (char *)"P", typeGeneratorPower);
        yices_var_decl dDecl = yices_mk_var_decl(ctx, (char *)"D", typeLoadPower);
        yices_var_decl yDecl = yices_mk_var_decl(ctx, (char *)"y", typeSelect);

        yices_expr f = yices_mk_var_from_decl(ctx, fDecl);
        yices_expr p = yices_mk_var_from_decl(ctx, pDecl);
        yices_expr d = yices_mk_var_from_decl(ctx, dDecl);
        yices_expr y = yices_mk_var_from_decl(ctx, yDecl);

        /*
         * Write Definitions on standard output
         */
        printf("(define f::(-> int int real))\n");
        printf("(define P::(-> int real))\n");
        printf("(define D::(-> int real))\n");
        printf("(define y::(-> int bool))\n");

        int i,j;
        const yices_expr ZERO = yices_mk_num(ctx, 0);

        /*
         * Make flow of all edges: (f i j)
         */
        for(i = 0; i < pgm -> nEdges; i++) {
            yices_expr from_to[2];
            from_to[0] = yices_mk_num(ctx, pgm -> edges[i].fromNode);
            from_to[1] = yices_mk_num(ctx, pgm -> edges[i].toNode);
            pgm -> edges[i].flow = yices_mk_app(ctx, f, from_to, 2);
            pgm -> edges[i].flowZeroCond = yices_mk_eq(ctx, pgm -> edges[i].flow, ZERO);
        }

        /*
         * Make power of all generators: (P i)
         */
        for( i = 0; i < pgm -> nGenerators; i++ ) {
            yices_expr argument = yices_mk_num(ctx, pgm -> generators[i].nodeID);
            pgm -> generators[i].power = yices_mk_app(ctx, p, &argument, 1);
            pgm -> generators[i].select = yices_mk_app(ctx, y, &argument, 1);
        }

        /*
         * Make power of all loads: (D i)
         */
        for( i = 0; i < pgm -> nLoads; i++ ) {
            yices_expr argument = yices_mk_num(ctx, pgm -> loads[i].nodeID);
            pgm -> loads[i].power = yices_mk_app(ctx, d, &argument, 1);
        }


        /*
         * Flow Conservative
         */
        for (i = 0; i < pgm -> nNodes; i++) {
            yices_expr left, right, *outgoings, *incomings, outgoingSum, incomingSum;

            outgoings = (yices_expr *) malloc((pgm -> nodes[i].nOutgoingEdges + 1) * sizeof (yices_expr));
            incomings = (yices_expr *) malloc((pgm -> nodes[i].nIncomingEdges + 1) * sizeof (yices_expr));

            /*
             * finding sum of outgoing and incoming edges
             * for simplicity we always add a Zero at the
             *  beginning of each sum, i.e. (+ 0 (f 0 1)...)
             */

            /*
             * Create ZERO terms !!!
             */
            outgoings[0] = ZERO;
            incomings[0] = ZERO;

            /*
             * Find the sum of outgoing edges (+ ZERO)
             */
            for (j = 0; j < pgm -> nodes[i].nOutgoingEdges; j++) {
                outgoings[j + 1] = pgm -> nodes[i].outgoingEdges[j] -> flow;
            }
            outgoingSum = yices_mk_sum(ctx, outgoings, pgm -> nodes[i].nOutgoingEdges + 1);
            free(outgoings);

            /*
             * Find the sum of incoming edges (+ ZERO)
             */
            for (j = 0; j < pgm -> nodes[i].nIncomingEdges; j++) {
                incomings[j + 1] = pgm -> nodes[i].incomingEdges[j] -> flow;
            }
            incomingSum = yices_mk_sum(ctx, incomings, pgm -> nodes[i].nIncomingEdges + 1);
            free(incomings);

            /*
             * Find left part of equation:
             * sum of outgoing flow - sum of incoming flow
             *          if i is generator or normal node
             * sum of incoming flow - sum of outgoing flow
             *          if i is load
             * Also find the right part of expression, that is:
             * P(i) if i is generator
             * D(i) if i is load
             * ZERO otherwise
             */

            if ( pgm -> nodes[i].nodeType == GENERATOR ) {
                yices_expr argLeft[2] = {outgoingSum, incomingSum};
                left = yices_mk_sub(ctx, argLeft, 2);
                right = pgm -> generators[pgm -> nodes[i].genOrLdIndex].power;
            }
            else if ( pgm -> nodes[i].nodeType == LOAD ) {
                yices_expr argLeft[2] = {incomingSum, outgoingSum};
                left = yices_mk_sub(ctx, argLeft, 2);
                right = pgm -> loads[pgm -> nodes[i].genOrLdIndex].power;
            }
            else {
                yices_expr argLeft[2] = {outgoingSum, incomingSum};
                left = yices_mk_sub(ctx, argLeft, 2);
                right = ZERO;
            }

            /*
             * ASSERT left == right
             */

            yices_expr flowConsv_i = yices_mk_eq(ctx, left, right);
            yices_assert(ctx, flowConsv_i);
            printf("(assert ");
            yices_pp_expr(flowConsv_i);
            printf(")\n");
        }

        /*
         * Flow capacity constraints
         */

        for(i = 0; i < pgm -> nEdges; i++) {
            char number[50];
            yices_expr bounds[2]; /* negative and positive part of the constraint */

            sprintf(number,"%f", (float)(-pgm -> edges[i].capacity ));
            bounds[0] = yices_mk_ge(ctx, pgm -> edges[i].flow, yices_mk_num_from_string(ctx, number));

            sprintf(number,"%f", (float)(pgm -> edges[i].capacity ));
            bounds[1] = yices_mk_le(ctx, pgm -> edges[i].flow, yices_mk_num_from_string(ctx, number));

            yices_expr flowConst_i = yices_mk_and(ctx, bounds, 2);
            yices_assert(ctx, flowConst_i);
            printf("(assert ");
            yices_pp_expr(flowConst_i);
            printf(")\n");
        }

        /*
         * Generator
         */
        for(i = 0; i < pgm -> nGenerators; i++) {
            yices_expr cond[2], either1[3], either2[2];
            char number[50];

            either1[0] = pgm -> generators[i].select;
            sprintf(number, "%f", pgm -> generators[i].minPower);
            either1[1] = yices_mk_ge(ctx, pgm -> generators[i].power, yices_mk_num_from_string(ctx, number));
            sprintf(number, "%f", pgm -> generators[i].maxPower);
            either1[2] = yices_mk_le(ctx, pgm -> generators[i].power, yices_mk_num_from_string(ctx, number));

            either2[0] = yices_mk_eq(ctx, pgm -> generators[i].power, ZERO);
            either2[1] = yices_mk_not(ctx, pgm -> generators[i].select);

            cond[0] = yices_mk_and(ctx, either1, 3);
            cond[1] = yices_mk_and(ctx, either2, 2);

            yices_expr genBound_i = yices_mk_or(ctx, cond, 2);
            yices_assert(ctx, genBound_i);
            printf("(assert ");
            yices_pp_expr(genBound_i);
            printf(")\n");
        }

        /*
         * Loads
         */
        for(i = 0; i < pgm -> nLoads; i++) {
            yices_expr cond[2];
            char number[50];

            sprintf(number, "%f", pgm -> loads[i].maxLoad);

            cond[0] = yices_mk_ge(ctx, pgm -> loads[i].power, ZERO);
            cond[1] = yices_mk_le(ctx, pgm -> loads[i].power, yices_mk_num_from_string(ctx, number));

            yices_expr loadBound_i = yices_mk_and(ctx, cond, 2);
            yices_assert(ctx, loadBound_i);
            printf("(assert ");
            yices_pp_expr(loadBound_i);
            printf(")\n");
        }

        /*
         * Load summation threshold
         */
        yices_expr sum, *loads;
        char lowerBoundStr[50];
        double lowerBound = 0;

        loads = (yices_expr *) malloc(pgm -> nLoads * sizeof(yices_expr *));
        for(i = 0; i < pgm -> nLoads; i++) {
            loads[i] = pgm -> loads[i].power;
            lowerBound += pgm -> loads[i].maxLoad;
        }
        lowerBound = threshold * lowerBound;
        sum = yices_mk_sum(ctx, loads, pgm -> nLoads);
        free(loads);

        sprintf(lowerBoundStr, "%lf", lowerBound);
        yices_expr loadSumThreshold = yices_mk_ge(ctx, sum, yices_mk_num_from_string(ctx, lowerBoundStr));
        yices_assert(ctx, loadSumThreshold);
        printf("(assert ");
        yices_pp_expr(loadSumThreshold);
        printf(")\n");


        return ctx;

    }

    void findMinCardinalityAttack(yices_context ctx, PowerGridModel* pgm) {
    	/*
    	 * Set yices into arithmetic only
    	 */
    		yices_set_arith_only(1);

        int subset[MAX_K];
        yices_expr offEdgeList[MAX_K];

        /* The number of removed edges */
        int k;

        /* for retracting previous asserts */
        int assertPlusCounter = 0;

        for( k = 2; k <= MAX_K; k++) {

            clock_t startMain = clock(), start, sumPushPop = 0, sumOffEdges = 0, sumChecking = 0;
            int isInconsistent = 0;
            /* Generate the first subset */
            for(int i = 0; i < k; i++)
                subset[i] = i;

            do {

            		start = clock();
            		yices_push(ctx);
            		sumPushPop += clock() - start;

            		start = clock();
                for(int i = 0; i < k; i++ ) {
                    /* Edge i is not in the network and has to have the flow of ZERO */
                    offEdgeList[i] = pgm -> edges[subset[i]].flowZeroCond;
                }

                /* Submit the command */
                yices_expr tt = yices_mk_and(ctx, offEdgeList, k);
                yices_assert(ctx, tt);
 //              int ras = yices_assert_retractable(ctx, tt);
 //             yices_pp_expr(tt);
                sumOffEdges += clock() - start;

                start = clock();
                /* Check Satisfiability */
                if(yices_check(ctx) == l_false)
                		isInconsistent = 1;
                sumChecking += clock() - start;

                start = clock();
                yices_pop(ctx);
//                yices_retract(ctx, ras);
        			sumPushPop += clock() - start;

            } while ( !isInconsistent && findNextSubset(subset, k, pgm -> nEdges ));

            printf("\nk = %d was done in %.3f Sec\n", k,(clock() - startMain)/(float)CLOCKS_PER_SEC );
            printf("Push/pop was done in %.3f Sec, submit off-edge assertion was done in %.3f Sec, inconsistency checking was done in %.3f Sec\n",
            (sumPushPop)/(float)CLOCKS_PER_SEC, (sumOffEdges)/(float)CLOCKS_PER_SEC, (sumChecking)/(float)CLOCKS_PER_SEC );
            if ( isInconsistent ) {
                printf("Minimum successful attack found. The attack size is %d. Vulnerable set is:\n{\n", k);
                for(int i = 0; i < k; i++) {
                		printf("\t%d => (%d , %d)\n", subset[i], pgm -> edges[i].fromNode, pgm -> edges[i].toNode);
                }
                printf("}\n");

                break;
            }


        }
		yices_set_arith_only(0);


    }

    void doAnalysis(char* networkModelFile, double threshold) {
    		PowerGridModel *pgm = readNetworkModel(networkModelFile);
    		yices_context ctx;
    		if(pgm)
    			ctx = createLogicalContext(pgm, threshold);
    		if(ctx)
    			findMinCardinalityAttack(ctx, pgm);

    		yices_del_context(ctx);
    		deleteNodeInPowerGridModel(pgm);
    		deletePowerGridModel(pgm);
    }

    /*
     ***************************************************************************
     *                           Using APILite
     ***************************************************************************
     */

    yicesl_context readYicesModel(char *fileName) {
        FILE *model = fopen(fileName, "r");

        if( !model ) {
            printf("Cannot open file %s", fileName);
            return NULL;
        }

        char command[MAX_COMMAND_LENGTH];

        yicesl_context ctx = yicesl_mk_context();

        if( !ctx ) {
            printf("yicesl_mk_context return an error. cannot creat logical contex.\n");
            fclose(model);
        }

    /*yicesl_read(ctx, "(set-verbosity! 2)");*/
        if(!yicesl_read(ctx, "(set-arith-only! true)")) {
            printf("cannot submit command: '(set-arith-only! true).\n'");
            fclose(model);
            /* Delete the context */
            yicesl_del_context(ctx);
        }

        while(!feof(model)) {
            fgets(command, MAX_COMMAND_LENGTH, model);
            if(!yicesl_read(ctx, command)) {
                printf("Cannot submit command: '%s'.\n",command);
                yicesl_del_context(ctx);
                ctx = NULL;
                break;
            }
        }

        fclose(model);

        return ctx;
    }

    int findNextSubset(int* c, int k, int n) {
        int i = k - 1;
        while ( i >= 0 && c[i] >= i + n - k )
            i--;

        if( i < 0 )
            return 0;
        c[i]++;
        int j;
        for( j = i + 1; j < k; j ++)
            c[j] = c[j-1] + 1;

        return 1;
    }

    struct PowerGridEdge* readEdges(char *fileName, int *nEdges) {
        FILE *edgeStructure = fopen(fileName, "r");


        if( !edgeStructure ) {
            printf("Cannot open file %s", fileName);
            return NULL;
        }

        fscanf(edgeStructure, "%d", nEdges);
        struct PowerGridEdge * edges = (struct PowerGridEdge *)malloc((*nEdges)*sizeof(struct PowerGridEdge));
        int i;
        for( i = 0; i < *nEdges; i++ ) {
            fscanf(edgeStructure,"%d%d", &edges[i].fromNode, &edges[i].toNode);
        }

        fclose(edgeStructure);

        return edges;



    }


#endif /* NERWORKANALYZER_H_ */
