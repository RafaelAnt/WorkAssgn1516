#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#include "papi.h"

#define NUM_EVENTS 4
#define MAX_RAND_NUMBER 100
#define MATRIX_SIZE_L1 40
#define MATRIX_SIZE_L2 120
#define MATRIX_SIZE_L3 600
#define MATRIX_SIZE_RAM 1000


//L1 cache = 32KB (por core)
//L2 cache = 256KB (por core)
//L3 cache = 6MB (partilhada)

float** transpose(float ** m,int n){
  float** aux;
  int i,j;

  if (( aux = malloc( n * sizeof( float* ))) == NULL )
    { printf("FATAL ERROR!\n"); }

  for ( i = 0; i < n; i++ ){
    if (( aux[i] = malloc( n * sizeof(float ) )) == NULL )
        { printf("FATAL ERROR!\n"); }
  }

  for(i=0; i<n; i++){
    for(j=0; j<n; j++){
      aux[i][j]=m[j][i];
    }
  }

  return aux;
}

/* Multiplicador de matrizes*/
void mmult(float **a, float **b, float **result, int n ) {
	int i, j, k;
  float** btrans;
	//#pragma vector always
	//#pragma ivdep
  btrans=transpose(b,n);
	for ( i = 0; i < n; i++){
		for ( j = 0; j < n; j++){
			for ( k = 0; k < n; k++){
				result[i][j] += a[i][k] * btrans[j][k];
			}
		}
	}
  free(btrans);
}

void printMat(float **mat, int n){
	int i, j;

	for( i = 0 ; i < n ; i++ ){
		for( j = 0 ; j < n ; j++ ){
			printf("%f ", mat[i][j]);
		}
		printf("\n");
	}
}

void clearCache(){
	float **flood = malloc(sizeof(float)*1000*1000);
	int i, j;
	float a;

	for(i=0; i<1000; i++){
		for(j=0; j<1000; j++){
			flood[i][j] = 321.5;
		}
	}

	for(i=0; i<1000; i++){
		for(j=0; j<1000; j++){
			a = flood[i][j];
		}
	}
	free(flood);
}

int createAndMult(int EventSet, int matrixSize){

	clearCache();
	//double * clearcache = malloc(sizeof(float)*10000*10000);

	//Inicializar variaveis
	int i, j;
 	float **matrizA;
 	float **matrizB;
 	float **matrizR; //Matriz resultado
	long long values[NUM_EVENTS];

	// Alocar Espaço para as matrizes
	if (( matrizA = malloc( matrixSize*sizeof( float* ))) == NULL )
		{ return 0; }
	if (( matrizB = malloc( matrixSize*sizeof( float* ))) == NULL )
		{ return 0; }
	if (( matrizR = malloc( matrixSize*sizeof( float* ))) == NULL )
		{ return 0; }

	for ( i = 0; i < matrixSize; i++ ){
		if (( matrizA[i] = malloc( matrixSize*sizeof(float ) )) == NULL )
				{ return 0; }
		if (( matrizB[i] = malloc( matrixSize*sizeof(float) )) == NULL )
				{ return 0; }
		if (( matrizR[i] = malloc( matrixSize*sizeof(float) )) == NULL )
				{ return 0; }
	}

	//Gerar matrizes com elementos aleatorios
	for ( i = 0; i < matrixSize; i++) {
	 for ( j = 0; j < matrixSize; j++) {
		 matrizA[i][j] = ((float) rand()) / (((float) RAND_MAX)*MAX_RAND_NUMBER);
		 matrizB[i][j] = ((float) rand()) / (((float) RAND_MAX)*MAX_RAND_NUMBER);
	 }
	}

	//iniciar papi
	PAPI_start(EventSet);
	// Iniciar contador de tempo
	double start = PAPI_get_real_usec();
	// calcular produto das matrizes
	mmult(matrizA, matrizB, matrizR, matrixSize);
	//finalizar contador de tempo
	double end = PAPI_get_real_usec();
	//Finalizar Papi
	PAPI_stop(EventSet,values);

	//imprimir resultados
	float segundos = (end-start)/1000000;
	long long int bytes = sizeof(float) * matrixSize * matrixSize;
	long long int flops = ((long long int)matrixSize * (long long int)matrixSize * (long long int)matrixSize) * 2;
	double gflops = flops/segundos;
	gflops = gflops/(1000000000);
	float opeationalIntensity = (float) flops/(64*values[2]);
	float ramAccesses = (float) values[2]/values[3];

	printf("%lld;%lld;%lld;%lld;%lld;%f;%f;%lld;%f;%f\n",
	bytes,					//bytes
	values[0],			//L1 Misses
	values[1],			//L2 Misses
	values[2],			//L3 Misses
	values[3],			//Total de Instruções
	ramAccesses,		//acessos à RAM por instrução
	segundos,				//Tempo em segundos
	flops,					// floating point operations
	gflops,					//giga flop per second
	opeationalIntensity
	);

	free(matrizA);
	free(matrizB);
	free(matrizR);
	//free(clearcache);
}

//*************************************************************
//
//                        MAIN FUNCTION
//
//*************************************************************
 int main() {
	 int i;

 	/*Eventos Papi*/
 	int Events[NUM_EVENTS]= {PAPI_L1_TCM, PAPI_L2_TCM, PAPI_L3_TCM, PAPI_TOT_INS};
 	int EventSet = PAPI_NULL, retval;

 	/* Initialize the Library */
 	retval = PAPI_library_init(PAPI_VER_CURRENT);
 	if (retval != PAPI_VER_CURRENT) {
 		fprintf(stderr, "PAPI library init error!\n");
 		exit(1);
 	}
 	/* Allocate space for the new eventset and do setup */
 	if (PAPI_create_eventset(&EventSet) != PAPI_OK) {
 		printf("Failed to allocate space for the new eventset and do setup\n ");
 		exit(0);
 	}
 	/* Add events to the eventset */
 	if (PAPI_add_events(EventSet,Events,NUM_EVENTS) != PAPI_OK) {
 		printf("Failed  Add events to the eventset \n");
 		exit(0);
 	}

	srand(0);

	for (i=0; i<8; i++){
		createAndMult(EventSet,MATRIX_SIZE_L1);
	}
	printf("\n");
	for (i=0; i<8; i++){
		createAndMult(EventSet,MATRIX_SIZE_L2);
	}
	printf("\n");
	for (i=0; i<8; i++){
		createAndMult(EventSet,MATRIX_SIZE_L3);
	}
	printf("\n");
	for (i=0; i<8; i++){
		createAndMult(EventSet,MATRIX_SIZE_RAM);
	}
	printf("\n");

 	return 1;
 }
