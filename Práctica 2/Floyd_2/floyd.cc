#include <iostream>
#include <fstream>
#include <string.h>
#include "Graph.h"
#include "mpi.h"
#include <math.h>
using namespace std;

//**************************************************************************

int main (int argc, char *argv[]){
	//Inicialización de variables
	int size, rank, rank_cartesiano;
	int raiz_p;
	int *buf_envio, *buf_recepcion;
	int nverts=G.vertices;
	double t=MPI_Wtime();
	raiz_p = sqrt(size);

   int dimensiones[] ={raiz_p, size/raiz_p}; //{filas comunicador, columnas comunicador}
   int periodos[] = {0, 0};//No sera periodica ninguna dimension.
   
	//===============================================

	MPI_Init(argc,argv);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

   MPI_Status status;
   MPI_Comm COMM_CART;//Guardaremos el comunicador cartesiano.
   MPI_Datatype subCamino;//Tipo de dato para una submatriz.

	if (argc != 2) {
		cerr << "Sintaxis: " << argv[0] << " <archivo de grafo>" << endl;
		MPI_Finalize();	
		return(-1);
	}

	Graph G;
	G.lee(argv[1]);		// Read the Graph
	//cout << "EL Grafo de entrada es:"<<endl;
	//G.imprime();

	if(G.vertices % sqrt(size) != 0){
		cerr << "Número de vertices no múltiplo de la raíz del número de procesos " << endl;
		MPI_Finalize();
		return (-1);
	}

	MPI_Cart_create(MPI_COMM_WORLD //A partir de los procesos de COMM_WORLD.
            ,2 //Creamos una malla de 2 dimensiones.
            ,dimensiones//Indicamos el tamaño de cada dimension.
            ,periodos//Indicamos la periodicidad de cada dimension.
            ,true//Permitimos que MPI organice los procesos.
            //ya no tienen porque tener el mismo rango que antes.
            , &COMM_CART);//Guardamos el nuevo comunicador.

	int tam_matraux = nverts / raiz_p ;

	int 2D[2];
	int origen; //Proceso cero dentro del comunicador cartesiano
   int matrizLocal[tam_matraux][tam_matraux];

	//Creación del tipo de dato bloque para empaquetar los datos
	MPI_Datatype MPI_BLOQUE;
	buf_envio = new int(nverts*nverts);

	if(rank==0){
		int *localMatriz_A;
		localMatriz_A = new int (nverts*nverts);

		for(int i=0; i<nverts; i++)
			for(int j=0; j<nverts; j++)
				localMatrizA[i*nverts+j]=G.A[i*nverts+j];

		MPI_Type_vector(tam_matraux,tam_matraux,nverts,MPI_INT,&MPI_BLOQUE);
		MPI_Type_commit(&MPI_BLOQUE);

		//EMPAQUETACIÓN
		for (int i=0,int posicion=0; i<size; i++){
			pivote_fila = i/raiz_p;
			pivote_columna = j%raiz_p;
			principio=(pivote_columna*tam_matraux)+(pivote_fila*tam_matraux*tam_matraux*raiz_p);

			MPI_Pack(matriz_A(comienzo),1,MPI_BLOQUE,buf_envio,sizeof(float)*nverts*nverts,&posicion, MPI_COMM_WORLD);
		}
		delete [] matriz_A;

		MPI_Type_free(&MPI_BLOQUE);
	}
	buf_recepcion = new int (tam_matraux*tam_matraux);

	/*Distribuimos la matriz entre los procesadores*/
	MPI_Scatter(buf_envio, sizeof(float)*tam_matraux*tam_matraux, MPI_PACKED, buf_recepcion, tam_matraux*tam_matraux, MPI_FLOAT, 0,MPI_COMM_WORLD);
	
	//Un comunicador por cada fila
	for(int i=0; i<tam_matraux; i++){

	}

	// BUCLE PPAL DEL ALGORITMO
	int i,j,k,vikj;
	for(k=0;k<nverts;k++)
	  {
		for(i=0;i<nverts;i++)
		  for(j=0;j<nverts;j++)
		   if (i!=j && i!=k && j!=k) 
		     {
		      vikj=G.arista(i,k)+G.arista(k,j);
		      vikj=min(vikj,G.arista(i,j));
		      G.inserta_arista(i,j,vikj);   
		     }
	  }
	  t=MPI::Wtime()-t;
	  MPI::Finalize();

	 
	  cout << endl<<"EL Grafo con las distancias de los caminos más cortos es:"<<endl<<endl;
	  G.imprime();
	  cout<< "Tiempo gastado= "<<t<<endl<<endl;


}



