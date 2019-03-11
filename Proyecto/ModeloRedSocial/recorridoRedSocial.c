/*
 * recorridoRedSocial: 
 * 	Example of two recursive implementations of depth-search graph traversing
 * 	to compute the minimum distance from a node to all the other nodes
 *
 * @author: Arturo Gonzalez-Escribano
 * @version 1.0
 * @date Feb 2019
 */
#include<stdio.h>
#include<limits.h>
#include<omp.h>
#include<string.h>
#include"digraph.h"

int total = 0;

/*
 * This function determines the number of matches between the most frequent hashtag
 */
int matches(const char *node1, const char *node2){
	int count = 0;
	for( int i = 0; i < strlen(node1); i++){
		for( int j = 0; j < strlen(node2); j++){
			if(node1[i] == node2[j]){
				count++;
			}
		}
	}
	return count;
}

/*
 * Compute the minimum distance from a node to any other node
 * Deep-first search, recursive implementation
 */
void computeMinimumDistance1( Digraph graph, int focusNode ) {
	/* Retrieve the minimum distance to this node */
	//int distance = graph.weight[ focusNode ];

	/* Traverse successors */
	int firstSucc = graph.edgesIndex[ focusNode ];
	int numSucc = graph.edgesIndex[ focusNode+1 ] - graph.edgesIndex[ focusNode ];
	int i;

	#pragma omp parallel for
	for(i=0; i<numSucc; i++ ) {
		int succ = graph.edges[ firstSucc+i ];
		/* Determine the number of matches */
		int matche = 1 + matches(graph.frequent[focusNode],graph.frequent[succ]);
		matche = 10*(7*matche-37)/(matche*matche*matche*matche-13*matche*matche*matche+64*matche*matche-132*matche+50);
		/* If equal weight, add 1 to the label */
		/*if (graph.weight[ succ ] == distance+1) {
			graph.label[ succ ]++;
		}*/
		
		if (graph.weight[ succ ] == matche) {
			graph.label[ succ ]++;
		}
		/* If shorter, update minimum distance in the child, and search it again */
		/*else if (graph.weight[ succ ] > distance+1) {
			graph.label[ succ ] = 1;
			graph.weight[ succ ] = distance+1;
			computeMinimumDistance1( graph, succ );
			total++; // Extra info: Count the number of total node explorations
		}*/
		else if (graph.weight[ succ ] > matche) {
			graph.label[ succ ] = 1;
			graph.weight[ succ ] = matche;
			#pragma omp task
			computeMinimumDistance1( graph, succ );
			total++; // Extra info: Count the number of total node explorations
		}
	}
}

/*
 * Compute the minimum distance from a node to any other node
 * Deep-first search, recursive implementation
 */
void computeMinimumDistance2( Digraph graph, int parent, int focusNode, int distance ) {
	total++; // Extra info: Count the number of total node explorations

	/* Retrieve the minimum distance to this node */
	int oldDistance = graph.weight[ focusNode ];

	/* If new distance is equal, add 1 to the label */
	if ( oldDistance == distance) {
		graph.label[ focusNode ]++;
	}
	/* If shorter, update minimum distance, and (re-)search children */
	else if ( oldDistance > distance ) {
		graph.label[ focusNode ] = 1;
		graph.weight[ focusNode ] = distance;

		/* Traverse successors */
		int firstSucc = graph.edgesIndex[ focusNode ];
		int numSucc = graph.edgesIndex[ focusNode+1 ] - graph.edgesIndex[ focusNode ];
		int i;
		#pragma omp parallel for
		for(i=0; i<numSucc; i++ ) {
			int succ = graph.edges[ firstSucc+i ];
			/* Determine the number of matches */
			int matche = 1 + matches(graph.frequent[focusNode],graph.frequent[succ]);
			matche = 10*(7*matche-37)/(matche*matche*matche*matche-13*matche*matche*matche+64*matche*matche-132*matche+50);
			#pragma omp task
			computeMinimumDistance2( graph, focusNode, succ, matche );
		}
	}
}

int main(int argc, char *argv[] ) {
	setbuf(stdout, NULL);

	/* Read aguments */
	if ( argc < 6 ) {
		fprintf(stderr, "Error -- Number of arguments\n");
		fprintf(stderr, "Usage: %s <numNodes> <meanEdgesPerNode> <textMin> <textMean> <longSeed>\n", argv[0] );
		exit( EXIT_FAILURE );
	}

	int numNodes = atoi( argv[1] );
	float connect = atof( argv[2] );
	int textMin = atoi( argv[3] );
	int textMean = atoi( argv[4] );
	long seed = atol( argv[5] );

	double timeCreate, timeInitMax, timeSearch, timePrint, timeValidation;

	/* Create graph */
	timeCreate = omp_get_wtime();
	Digraph graph = digraphRandom( numNodes, connect, textMin, textMean, seed );
	timeCreate = omp_get_wtime() - timeCreate;

	/* Initialize minimum distance to INT_MAX */
	timeInitMax = omp_get_wtime();
	int i;
	for( i=0; i<numNodes; i++ ) graph.weight[i] = INT_MAX;
	timeInitMax = omp_get_wtime() - timeInitMax;

	/* Search starting on an arbitrary node */
	int startNode = numNodes / 2;
	
	timeSearch = omp_get_wtime();
#ifdef VARIANTE_1
	graph.weight[ startNode ] = 10;
	computeMinimumDistance1( graph, startNode );
#else // VARIANTE_2
	computeMinimumDistance2( graph, startNode, startNode, 10 );
#endif
	timeSearch = omp_get_wtime() - timeSearch;

	/* Print the whole resulting graph */
	timePrint = omp_get_wtime();
	//digraphPrint( graph );
	timePrint = omp_get_wtime() - timePrint;	

	/* Extra info: Total number of node explorations */
	printf("Total node explorations: %d\n", total );

	/* Valitaion */
	int s = 0;
	timeValidation = omp_get_wtime();
	#pragma omp parallel for reduction(+:s)
	for(i=0; i<graph.numNodes; i++){
		s = s + graph.weight[i];
	}
	timeValidation = omp_get_wtime() - timeValidation;

	/* Extra info: Total number of node explorations */
	printf("El total de las distancias es: %d\n", s );

	/* Timers */
	double timeTotal = timeCreate + timeInitMax + timeSearch + timePrint + timeValidation;
	printf("Time create:		%12.8lf, %6.2lf %%\n", timeCreate, timeCreate/timeTotal * 100 );
	printf("Time initMax:		%12.8lf, %6.2lf %%\n", timeInitMax, timeInitMax/timeTotal * 100 );
	printf("Time search:		%12.8lf, %6.2lf %%\n", timeSearch, timeSearch/timeTotal * 100 );
	printf("Time print		%12.8lf, %6.2lf %%\n", timePrint, timePrint/timeTotal * 100 );
	printf("Time validation:	%12.8lf, %6.2lf %%\n", timeValidation, timeValidation/timeTotal * 100 );
	printf("Time total:		%12.8lf, %6.2lf %%\n", timeTotal, 100.0 );

	return 0;
}
