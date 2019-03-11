/*
 * Digraph: Data structure for directed graphs
 *
 * @author: Arturo Gonzalez-Escribano
 * @version 2.0
 * @date Feb 2018
 */
#ifndef _DIGRAPH_
#define _DIGRAPH_

#include<stdio.h>
#include<stdlib.h>

/*
 * Struct type: Digraph (Directed Graph)
 */
typedef struct Digraph {
	int numNodes;
	int *edgesIndex;
	int *edges;
	int *numPred;
	int *label;
	int *flag;
	int *weight;
	char **text;
	char **frequent;
} Digraph;

/*
 * Free digraph
 */
void digraphFree( Digraph * g );

/*
 * Clean digraph annotations (labels and weights)
 */
void digraphCleanAnnotations( Digraph * g );

/*
 * Generate digraph
 */
Digraph digraphRandom( int numNodes, 
				float neighborsMean,
				int textMin, 
				int textMax,
				long seed );

/* 
 * Generate digraph
 */
void digraphPrint( Digraph di );

#endif /* _DIGRAPH_ */
