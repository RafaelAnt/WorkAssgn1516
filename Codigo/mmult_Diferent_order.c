#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#include "papi.h"

#define NUM_EVENTS 4
#define MAX_RAND_NUMBER 100

//L1 cache = 32KB (por core)
//L2 cache = 256KB (por core)
//L3 cache = 6MB (partilhada)

/* Multiplicador de matrizes*/

void mmult(float **a, float **b, float **result, int n ) {
	int i, j, k;
	float r;

	for ( j = 0; j < n; j++)
		for ( i = 0; i < n; i++) {
			r=0;
	 	 	for ( k = 0; k < n; k++)
       			r += a[i][k] * b[k][j];
			result[i][j] =r;
	}

}


/*
 * recebebe como parametros (altura e largura da 1� matriz)
 */
int main(int argc, char *argv[]) {

	if(argc != 2){
		printf("ARGUMENT_ERROR\n");
		return -1;
	}


	int matrixSize = atoi(argv[1]);
	int i, j;

	//printf("%d\n",matrixSize);

	/*Inicializar variaveis*/
	float **matrizA;
	float **matrizB;
	float **matrizR;/*Matriz resultado*/

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

	/*Gerar matrizes com elementos aleatorios*/

  for ( i = 0; i < matrixSize; i++) {
    for ( j = 0; j < matrixSize; j++) {
      matrizA[i][j] = ((float) rand()) / (((float) RAND_MAX)*MAX_RAND_NUMBER);
      matrizB[i][j] = ((float) rand()) / (((float) RAND_MAX)*MAX_RAND_NUMBER);
    }
  }



	/*iniciar papi*/
	PAPI_start(EventSet);
	/* Iniciar contador de tempo*/

	double start = PAPI_get_real_usec();

	// calcular produto das matrizes
	mmult(matrizA, matrizB, matrizR, matrixSize);

	/*finalizar contador de tempo*/
	double end = PAPI_get_real_usec();

	/*Finalizar Papi*/
	PAPI_stop(EventSet,values);



	/*imprimir resultados*/
	float segundos = (end-start)/1000000;
	long long int bytes = sizeof(float) * matrixSize * matrixSize;
	long long int flops = ((long long int)matrixSize * (long long int)matrixSize * (long long int)matrixSize) * 3;
	double gflops = flops/segundos;
	gflops = gflops/(1000000000);
	double opeationIntensity = flops/bytes;

	//printf("MatrizSize = %d\n", matrixSize);
	//printf("MatrizSize = %lld\n", flops);

	printf("%lld,%lld,%lld,%lld,%lld,%f,%lld,%f,%f;\n",
	bytes,
	values[0],
	values[1],
	values[2],
	values[3],
	segundos,
	flops,
	gflops,
	opeationIntensity
  );

	return 1;
}
