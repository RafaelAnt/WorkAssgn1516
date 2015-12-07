#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#include "papi.h"

#define NUM_EVENTS 3
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
  float aux;
	//#pragma vector always
	//#pragma ivdep
  btrans=transpose(b,n);
	for ( i = 0; i < n; i++){
		for ( j = 0; j < n; j++){
			aux = 0;
			for ( k = 0; k < n; k++){
				aux += a[i][k] * btrans[j][k];
			}result[i][j] = aux;
			
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
	const int size =20*1024*1024;
	char *c = (char *)malloc(size);
	for (int i =0; i<0xffff; i++)
		for(int j =0; j<size;j++)
			c[j] = i*j;
}

void createAndMult(int matrixSize){

	
	//double * clearcache = malloc(sizeof(float)*10000*10000);

	//Inicializar variaveis
	int i, j;
 	float **matrizA;
 	float **matrizB;
 	float **matrizR; //Matriz resultado
	long long values[NUM_EVENTS];
	
	/*Eventos Papi*/
 	int Events[NUM_EVENTS]= {PAPI_L1_TCM, PAPI_L2_TCM, PAPI_L3_TCM};
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

	
	
	// Alocar Espaço para as matrizes
	if (( matrizA = malloc( matrixSize*sizeof( float* ))) == NULL )
		{ return; }
	if (( matrizB = malloc( matrixSize*sizeof( float* ))) == NULL )
		{ return; }
	if (( matrizR = malloc( matrixSize*sizeof( float* ))) == NULL )
		{ return; }

	for ( i = 0; i < matrixSize; i++ ){
		if (( matrizA[i] = malloc( matrixSize*sizeof(float ) )) == NULL )
				{ return; }
		if (( matrizB[i] = malloc( matrixSize*sizeof(float) )) == NULL )
				{ return; }
		if (( matrizR[i] = malloc( matrixSize*sizeof(float) )) == NULL )
				{ return; }
	}

	//Gerar matrizes com elementos aleatorios
	for ( i = 0; i < matrixSize; i++) {
	 for ( j = 0; j < matrixSize; j++) {
		 matrizA[i][j] = ((float) rand()) / (((float) RAND_MAX)*MAX_RAND_NUMBER);
		 matrizB[i][j] = ((float) rand()) / (((float) RAND_MAX)*MAX_RAND_NUMBER);
	 }
	}
	clearCache();
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
	double segundos = (end-start)/1000000.0f;
	long long int bytes = sizeof(float) * matrixSize * matrixSize;
	long long int flops = ((long long int)matrixSize * (long long int)matrixSize * (long long int)matrixSize) * 2;
	double gflops = flops/segundos;
	gflops = gflops/(1000000000);
	float opeationalIntensity = (float) flops/(64*values[2]);
	float ramAccesses = (float) values[1]/values[2];

	printf("%lld;%lld;%lld;%lld;%f;%f;%lld;%f;%f\n",
	bytes,					//bytes
	values[0],			//L1 Misses
	values[1],			//L2 Misses
	values[2],			//L3 Misses
	//values[3],			//Total de Instruções
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

	srand(0);
 	

	for (i=0; i<8; i++){
		createAndMult(MATRIX_SIZE_L1);
		srand(i);
	}
	printf("\n");
	for (i=0; i<8; i++){
		createAndMult(MATRIX_SIZE_L2);
	}
	printf("\n");
	for (i=0; i<8; i++){
		createAndMult(MATRIX_SIZE_L3);
	}
	printf("\n");
	for (i=0; i<8; i++){
		createAndMult(MATRIX_SIZE_RAM);
	}
	printf("\n");

 	return 1;
 }
