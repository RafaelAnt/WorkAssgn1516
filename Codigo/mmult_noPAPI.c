#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <omp.h>

//#include "papi.h"


#define NUM_EVENTS 4
#define MAX_RAND_NUMBER 100
#define MATRIX_SIZE 10000




/* Multiplicador de matrizes*/
/*
void mmult(int **a, int **b, int **result, int n ) {
	int i, j, k;
	
	for ( i = 0; i < n; i++)
    for ( j = 0; j < n; j++)
	 	 for ( k = 0; k < n; k++)
       result[i][j] += a[i][k] * b[k][j];
}
*/

/*
 * recebebe como parametros (altura e largura da 1ª matriz)
 */
int main() {
	

	printf("Teste");


	int matrizSize = MATRIX_SIZE;
	int i, j;
	
	printf("Teste");
	
	/*Inicializar variaveis*/
	int **matrizA; 
	int **matrizB;
	int **matrizR;/*Matriz resultado*/
	
	/*Eventos Papi*/
	/*int Events[NUM_EVENTS]= {PAPI_L1_TCM, PAPI_L2_TCM, PAPI_L3_TCM, PAPI_TOT_INS};
	int EventSet;
	long long values[NUM_EVENTS];
	/* Initialize the Library */
	/*PAPI_library_init(PAPI_VER_CURRENT);
	/* Allocate space for the new eventset and do setup */
	//PAPI_create_eventset(&EventSet);
	/* Add Flops and total cycles to the eventset */
	//PAPI_add_events(EventSet,Events,NUM_EVENTS);
	

	if (( matrizA = malloc( MATRIX_SIZE*sizeof( int* ))) == NULL )
		{ return 0; }
	if (( matrizB = malloc( MATRIX_SIZE*sizeof( int* ))) == NULL )
		{ return 0; }
	if (( matrizR = malloc( MATRIX_SIZE*sizeof( int* ))) == NULL )
		{ return 0; }

	for ( i = 0; i < MATRIX_SIZE; i++ ){
	  	if (( matrizA[i] = malloc( sizeof(int) )) == NULL )
		  	{ return 0; }
		if (( matrizA[i] = malloc( sizeof(int) )) == NULL )
		  	{ return 0; }
		if (( matrizA[i] = malloc( sizeof(int) )) == NULL )
		  	{ return 0; }		  
	}

printf("Teste- Alocou a matriz");
	
	
	/*Gerar matrizes com elementos aleatorios*/
	
  for ( i = 0; i < MATRIX_SIZE; ++i) {
    for ( j = 0; j < MATRIX_SIZE; ++j) {
      matrizA[i][j] = ((float) rand()) / (((float) RAND_MAX)*MAX_RAND_NUMBER);
      matrizB[i][j] = ((float) rand()) / (((float) RAND_MAX)*MAX_RAND_NUMBER);
    }
  }

printf("Teste - Adicionou os random");
	
	/* Iniciar contador de tempo*/
	double start = omp_get_wtime();
	
	/*iniciar papi*/
	//PAPI_start(EventSet);
	
	// calcular produto das matrizes
	//mmult(matrizA, matrizB, matrizR, matrizSize);

printf("Teste - calculou a matriz");
	
	/*Finalizar Papi*/
	//PAPI_stop(EventSet,values);
	
	/*finalizar contador de tempo*/
	double end = omp_get_wtime();
	
	/*imprimir resultados*/
	
	//printf("PAPI_L1_TCM= %lli\n PAPI_L2_TCM = %lli\n PAPI_L3_TCM = %lli\n PAPI_TOT_INS = %lli\n", values[0],values[1],values[2],values[3]);
	
	printf("Cocluido em %f segundos.\n", (end-start));
	
	
	return 1;
}
