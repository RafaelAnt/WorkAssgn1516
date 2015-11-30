#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <omp.h>

#include <papi.h>


#define NUM_EVENTS 4
#define MAX_RAND_NUMBER 100
//#define MATRIX_SIZE 1000

//L1 cache = 32KB (por core)
//L2 cache = 256KB (por core)
//L3 cache = 6MB (partilhada)


/* Multiplicador de matrizes*/

void mmult(float **a, float **b, float **result, int n ) {
	int i, j, k;

	for ( i = 0; i < n; i++)
    for ( j = 0; j < n; j++)
	 	 for ( k = 0; k < n; k++)
       result[i][j] += a[i][k] * b[k][j];
}


/*
 * recebebe como parametros (altura e largura da 1ª matriz)
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



 	/* Iniciar contador de tempo*/
 	double start = omp_get_wtime();

 	/*iniciar papi*/
 	PAPI_start(EventSet);

 	// calcular produto das matrizes
 	mmult(matrizA, matrizB, matrizR, matrixSize);



 	/*Finalizar Papi*/
 	PAPI_stop(EventSet,values);

 	/*finalizar contador de tempo*/
 	double end = omp_get_wtime();

 	/*imprimir resultados*/
 	//printf("\n\n");
 	//printf("Tamanho de cada matriz: ");
 	  long long int bytes = sizeof(float) * matrixSize * matrixSize;
 	  //if(bytes<=1024)  printf("%.3f bytes...\n", (double) bytes);
 	  //if(bytes>1024 && bytes <= 1024*1024)  printf("%.3f Kbytes...\n", (double) bytes/1024);
 	  //if(bytes>1024*1024)  printf("%.3f Mbytes...\n", (double) bytes/(1024*1024));
 		//bytes = bytes * 3;
 	//printf("Tamanho Total: ");
 		//if(bytes<=1024)  printf("%.3f bytes...\n", (double) bytes);
 		//if(bytes>1024 && bytes <= 1024*1024)  printf("%.3f Kbytes...\n", (double) bytes/1024);
 		//if(bytes>1024*1024)  printf("%.3f Mbytes...\n", (double) bytes/(1024*1024));


 	//printf("\nTamanho de cada matriz = %ld Bytes\n\n",sizeof(float)*matrixSize*matrixSize);
 	//printf("\nPAPI_L1_TCM= %lld\nPAPI_L2_TCM = %lld\nPAPI_L3_TCM = %lld\nPAPI_TOT_INS = %lld\n\n", values[0],values[1],values[2],values[3]);

 	//printf("Concluido em %f segundos.\n\n", (end-start));

 	printf("%lld,%lld,%lld,%lld,%lld,%f;\n",bytes,values[0],values[1],values[2],values[3],(end-start));

 	return 1;
 }
