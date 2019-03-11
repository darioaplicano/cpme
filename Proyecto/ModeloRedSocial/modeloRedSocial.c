/*
 * modeloRedSocial: Example of creating and showing a digraph model of social network
 *
 * @author: Arturo Gonzalez-Escribano
 * @version 1.0
 * @date Feb 2019
 */
#include<stdio.h>
#include"digraph.h"

int main(int argc, char *argv[] ) {

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

	Digraph graph = digraphRandom( numNodes, connect, textMin, textMean, seed );

	digraphPrint( graph );

	return 0;
}
