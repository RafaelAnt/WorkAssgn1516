#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <omp.h>

#include "papi.h"


#define NUM_EVENTS 4
#define MAX_RAND_NUMBER 100
#define MATRIX_SIZE 1200

//L1 cache = 32KB (por core)
//L2 cache = 256KB (por core)
//L3 cache = 6MB (partilhada)

//Size 50 = 19.531 KB
//Size 165 = 212.695 KB
//Size 800 = 2.85481 MB
//Size 1200 = 10.986 MB


/* Multiplicador de matrizes*/

void mmult(int **a, int **b, int **result, int n ) {
	int i, j, k;

	for ( j = 0; j < n; j++)
		for ( i = 0; i < n; i++)
	 	 	for ( k = 0; k < n; k++)
       			result[i][j] += a[i][k] * b[k][j];
}


/*
 * recebebe como parametros (altura e largura da 1� matriz)
 */
int main(int argc, char *argv[]) {


	int matrizSize = MATRIX_SIZE;
	int i, j;



	/*Inicializar variaveis*/
	int **matrizA;
	int **matrizB;
	int **matrizR;/*Matriz resultado*/

	/*Eventos Papi*/
	int Events[NUM_EVENTS]= {PAPI_L1_TCM, PAPI_L2_TCM, PAPI_L3_TCM, PAPI_TOT_INS};
	int EventSet = PAPI_NULL, retval;
	long long values[NUM_EVENTS];
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


	if (( matrizA = malloc( MATRIX_SIZE*sizeof( int* ))) == NULL )
		{ return 0; }
	if (( matrizB = malloc( MATRIX_SIZE*sizeof( int* ))) == NULL )
		{ return 0; }
	if (( matrizR = malloc( MATRIX_SIZE*sizeof( int* ))) == NULL )
		{ return 0; }

	for ( i = 0; i < MATRIX_SIZE; i++ ){
	  	if (( matrizA[i] = malloc( MATRIX_SIZE*sizeof(int ) )) == NULL )
		  	{ return 0; }
		if (( matrizB[i] = malloc( MATRIX_SIZE*sizeof(int) )) == NULL )
		  	{ return 0; }
		if (( matrizR[i] = malloc( MATRIX_SIZE*sizeof(int) )) == NULL )
		  	{ return 0; }
	}




	/*Gerar matrizes com elementos aleatorios*/

  for ( i = 0; i < MATRIX_SIZE; ++i) {
    for ( j = 0; j < MATRIX_SIZE; ++j) {
      matrizA[i][j] = ((float) rand()) / (((float) RAND_MAX)*MAX_RAND_NUMBER);
      matrizB[i][j] = ((float) rand()) / (((float) RAND_MAX)*MAX_RAND_NUMBER);
    }
  }



	/* Iniciar contador de tempo*/
	double start = omp_get_wtime();

	/*iniciar papi*/
	PAPI_start(EventSet);

	// calcular produto das matrizes
	mmult(matrizA, matrizB, matrizR, matrizSize);



	/*Finalizar Papi*/
	PAPI_stop(EventSet,values);

	/*finalizar contador de tempo*/
	double end = omp_get_wtime();

	/*imprimir resultados*/
	printf("\n\n");
	printf("Tamanho de cada matriz: ");
	  double bytes = sizeof(float) * MATRIX_SIZE * MATRIX_SIZE;
	  if(bytes<=1024)  printf("%.3f bytes...\n", (double) bytes);
	  if(bytes>1024 && bytes <= 1024*1024)  printf("%.3f Kbytes...\n", (double) bytes/1024);
	  if(bytes>1024*1024)  printf("%.3f Mbytes...\n", (double) bytes/(1024*1024));
		bytes = bytes * 2;
	printf("Tamanho Total: ");
		if(bytes<=1024)  printf("%.3f bytes...\n", (double) bytes);
		if(bytes>1024 && bytes <= 1024*1024)  printf("%.3f Kbytes...\n", (double) bytes/1024);
		if(bytes>1024*1024)  printf("%.3f Mbytes...\n", (double) bytes/(1024*1024));


	//printf("\nTamanho de cada matriz = %ld Bytes\n\n",sizeof(float)*MATRIX_SIZE*MATRIX_SIZE);
	printf("\nPAPI_L1_TCM= %lld\n PAPI_L2_TCM = %lld\n PAPI_L3_TCM = %lld\n PAPI_TOT_INS = %lld\n\n", values[0],values[1],values[2],values[3]);

	printf("Concluido em %f segundos.\n\n", (end-start));


	return 1;
}
