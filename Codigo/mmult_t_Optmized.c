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



float* transpose(float * m, int n){
  float* aux;
  int i,j;

  aux = malloc( n * n * sizeof( float ));

  for(i=0; i<n; i++){
    for(j=0; j<n; j++){
      aux[i * n + j]=m[j * n + i];
    }
  }

  return aux;
}

/* Multiplicador de matrizes*/
void mmult(float *  a, float *  b, float *  result, int n ) {
	int i, j, k, auxi, auxj;
	float* btrans;
	float aux;
	btrans=transpose(b,n);


	for ( i = 0; i < n; i++){
	
		for ( j = 0; j < n; j++){
			aux = 0;
			auxi = i*n;
			auxj = j*n;	
						
			for ( k = 0; k < n; k++){
				
				aux = aux +  a[auxi + k] * btrans[auxj + k];
			}
			result[auxi+j] = aux;	
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

float clearCache(){
	float flood[10000][10000];
	int i, j;
	float a;

	for(i=0; i<10000; i++){
		for(j=0; j<10000; j++){
			flood[i][j] = 321.5;
		}
	}

	for(i=0; i<10000; i++){
		for(j=0; j<10000; j++){
			a = flood[i][j];
		}
		
	}
	//free(flood);
	return a;
}

void createAndMult(int EventSet, int matrixSize){
	
	
	//double * clearcache = malloc(sizeof(float)*10000*10000);

	//Inicializar variaveis
	int i, j;
 	float *matrizA;
 	float *matrizB;
 	float *matrizR; //Matriz resultado
	long long values[NUM_EVENTS];

	// Alocar Espaço para as matrizes

  matrizA = (float*) malloc( matrixSize * matrixSize * sizeof( float ));
  matrizB = (float*) malloc( matrixSize * matrixSize * sizeof( float ));
  matrizR = (float*) malloc( matrixSize * matrixSize * sizeof( float ));

//printf("Cheguei\n");
	//Gerar matrizes com elementos aleatorios
	for ( i = 0; i < matrixSize; i++) {
	 for ( j = 0; j < matrixSize; j++) {
     //printf("i=%d     j=%d   \n",i,j );
		 matrizA[i * matrixSize + j] = ((float) rand()) / (((float) RAND_MAX)*MAX_RAND_NUMBER);
		 matrizB[i * matrixSize + j] = ((float) rand()) / (((float) RAND_MAX)*MAX_RAND_NUMBER);
	 }
	}
	//printf("Cheguei, vou fazer clear a cache\n");
	//printf("Cheguei, vou começar\n");
	clearCache();
	//printf("Cheguei, vou começar\n");
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
//printf("Cheguei, vou começar\n");
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
