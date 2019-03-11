/*
 * Digraph: Data structure for directed graphs
 *
 * @author: Arturo Gonzalez-Escribano
 * @version 1.1
 * @date Feb 2019
 */
#include"digraph.h"
#include<string.h>
#include<math.h>

/*
 * Free digraph
 */
void digraphFree( Digraph * g ) {
	int i;
	free( g->edgesIndex );
	free( g->edges );
	free( g->numPred );
	free( g->label );
	free( g->weight );
	for( i=0; i<g->numNodes; i++ ) free( g->text[i] );
	g->numNodes = 0;
	g->edgesIndex = NULL;
	g->edges = NULL;
	g->numPred = NULL;
	g->label = NULL;
	g->flag = NULL;
	g->weight = NULL;
	g->text = NULL;
}

/*
 * Clean digraph annotations (labels and weights)
 */
void digraphCleanAnnotations( Digraph * g ) {
	free( g->label );
	free( g->weight );
	free( g->flag );
	g->label = (int *)calloc( g->numNodes, sizeof(int) );
	g->weight = (int *)calloc( g->numNodes, sizeof(int) );
	g->flag = (int *)calloc( g->numNodes, sizeof(int) );
}

//Cuenta cuantas veces aparece una letra en una cadena
int countCharacterR(char *c, char *j){
	int cant = 0;
	while(*c != '\0'){
		if(*c == *j)
			cant++;
		c++;
	}
	return cant;
}

//Ordena el arreglo de los caracteres y del número de veces que aparecen
void sortArray(int *cant, char **alfa){
	int intercambio;
	char *interCambio;
	for (int i = 0; i < 27; i++){
		for( int j = i+1; j < 27; j++){
			if( cant[i] < cant[j]){
				intercambio = cant[j];
				interCambio = alfa[j];
				cant[j] = cant[i];
				alfa[j] = alfa[i];
				cant[i] = intercambio;
				alfa[i] = interCambio;
			}
		}
	}
}

/*
 * Generate random digraph
 */
Digraph digraphRandom( int numNodes, 
				float neighborsMean,
				int textMin, 
				int textMean,
				long seed ) {
	Digraph new = { 0, NULL, NULL, NULL, NULL, NULL, NULL };

	/* Memory for the basic structure of nodes */
	new.numNodes = numNodes;
	new.edgesIndex = (int *)malloc( (new.numNodes+1) * sizeof(int) );
	new.numPred = (int *)calloc( (size_t)new.numNodes, sizeof(int) );
	new.label = (int *)calloc( (size_t)new.numNodes, sizeof(int) );
	new.weight = (int *)calloc( (size_t)new.numNodes, sizeof(int) );
	new.flag = (int *)calloc( (size_t)new.numNodes, sizeof(int) );
	new.text = (char **)malloc( (size_t)new.numNodes * sizeof(char *) );
	new.frequent = (char **)malloc( (size_t)new.numNodes * sizeof(char *) );

	if ( new.weight == NULL 
			|| new.edgesIndex == NULL 
			|| new.numPred == NULL 
			|| new.label == NULL 
			|| new.text == NULL 
			) {
		perror("Digraph allocating memory 1\n");
		exit(EXIT_FAILURE);
	}

	/* Create random edges */
	srand48( seed );
	new.edges = NULL;

	new.edgesIndex[0] = 0;
	int totalEdges = 0;
	int i,j;
	for ( i=0; i<new.numNodes; i++ ) {
		new.edgesIndex[i+1] = new.edgesIndex[i];

		/* Exponential distribution */
		int numberOfEdges = - neighborsMean * log( 1 - drand48() );
		float prob = (float)numberOfEdges / (float)new.numNodes;
		if (prob > 0.95 ) prob = .95;
		for ( j=0; j<new.numNodes; j++ ) {
			if ( i == j ) continue; // Skip self edges
			if ( drand48() < prob ) {
				new.edgesIndex[i+1]++;
				totalEdges++;
				new.edges = (int *)realloc( new.edges, sizeof(int) * (size_t)totalEdges );
				new.edges[totalEdges-1] = j;
				new.numPred[j]++;
			}
		}
	}

	/* Create random text pieces */
	for ( i=0; i<new.numNodes; i++ ) {
		/* Exponential distribution */
		int expMean = textMean - textMin;
		int textSize = -expMean * log( 1 - drand48() ) + textMin;

		new.text[i] = (char *)malloc( (size_t)textSize + 1 );
		for ( j=0; j<textSize; j++ ) {
			char word = (char)(drand48() * 27) + '@';
			new.text[i][j] = word;
		}
		new.text[i][textSize] = '\0';
	}

	/* Find the ones that repeat the most */
	int moreFrequent = 5;
	for(i = 0; i < new.numNodes; i++){
		//Create the new instance about frequent in the text
		new.frequent[i] = (char *)malloc( (size_t)moreFrequent + 1 );

		char *alpha[27] = {"A","B","C","D","E","F","G","H","I","J","K","L","M","N","O","P","Q","R","S","T","U","V","W","X","Y","Z","@"};
		int quantity[27]; 

		for(j=0; j<27; j++){
			quantity[j] = countCharacterR(new.text[i], alpha[j]);
		}

		sortArray(quantity, alpha);
		for(j = 0; j < moreFrequent; j++){
			strcat(new.frequent[i],alpha[j]);
		}
	}

	return new;
}

/*
 * Print digraph
 */
void digraphPrint( Digraph di ) {
	printf("-- Digraph -------\n");
	printf("-- Num nodes: %d\n", di.numNodes );

	int i;
	for( i=0; i<di.numNodes; i++ ) {
		/* Children edges */
		int numSucc = di.edgesIndex[i+1] - di.edgesIndex[i];
		int firstSucc = di.edgesIndex[i];
		printf("-- Node:   %d\n", i );
		printf("-- Pred:   %d\n", di.numPred[i] );
		printf("-- Label:  %d\n", di.label[i] );
		printf("-- Weight: %d\n", di.weight[i] );
		printf("-- Flag: %d\n", di.flag[i] );
		printf("-- Succ:   %d =", numSucc );
		int j;
		for ( j=0; j<numSucc; j++ ) {
			printf(" %d", di.edges[ firstSucc+j ] );
		}
		printf("\n");

		/* Text */
		printf("-- Text: length=%ld, content=\"%s\"\n", strlen(di.text[i]), di.text[i] );

		/* Frequent */
		printf("-- Frequent: \"%s\"\n", di.frequent[i]);
		printf("--\n");
	}

}