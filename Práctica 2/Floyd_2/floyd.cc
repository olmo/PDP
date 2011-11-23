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
	
	int dimensiones[] = {raiz_p, size/raiz_p}; //{filas comunicador, columnas comunicador}
	int periodos[] = {0, 0};//No sera periodica ninguna dimension.

	MPI_Cart_create(MPI_COMM_WORLD, 2, dimensiones, periodos, true, &COMM_CART);

	int tam = nverts / raiz_p ;

	int coord[2];
	int origen; //Proceso cero dentro del comunicador cartesiano

	//Creación del tipo de dato bloque para empaquetar los datos
	MPI_Datatype bloque;
	buf_envio = new int(nverts*nverts);
	
	int rank_hor, rank_ver;

	if(rank==0){
		Graph G;
		G.lee(argv[1]);		// Read the Graph
		//cout << "EL Grafo de entrada es:"<<endl;
		//G.imprime();

		if(G.vertices % sqrt(size) != 0){
			cerr << "Número de vertices no múltiplo de la raíz del número de procesos " << endl;
			MPI_Finalize();
			return (-1);
		}

		MPI_Type_vector(tam, tam, nverts, MPI_INT, &bloque);
		MPI_Type_commit(&bloque);

		//Empaquetamiento
		int posicion=0, fila_p, columna_p, comienzo;
		for (int i=0; i<size; i++){
			fila_p = i/raiz_p;
			columna_p = j%raiz_p;
			comienzo=(columna_p*tam)+(fila_p*tam*tam*raiz_p);

			MPI_Pack(G.A[comienzo], 1, MPI_bloque, buf_envio, sizeof(float)*nverts*nverts, &posicion,  MPI_COMM_WORLD);
		}
		
		//delete [] matriz_A;
		MPI_Type_free(&bloque);
	}
	
	buf_recepcion = new int (tam*tam);

	/*Distribuimos la matriz entre los procesadores*/
	MPI_Scatter(buf_envio, sizeof(float)*tam*tam, MPI_PACKED, buf_recepcion, tam*tam, MPI_FLOAT, 0,MPI_COMM_WORLD);
	
	//Un comunicador por cada fila
	MPI_Comm horizontal, vertical;
	MPI_Comm_split(MPI_COMM_WORLD, rank/tam, 0, comm_horizontal);
	MPI_Comm_split(MPI_COMM_WORLD, rank%tam, 0, comm_vertical);
	
	MPI_Comm_rank(comm_horizontal, &rank_hor);
	MPI_Comm_rank(comm_vertical, &rank_ver);

	// BUCLE PPAL DEL ALGORITMO
	double t=MPI_Wtime();
	
	int *filak = new int[tam];
	int *columna = new int[tam];
	int i,j,k,vikj;
	for(k=0;k<nverts;k++){
		if(rank == k%tam){
			for (int i=0; i<tam; i++)
				filak[i] = buf_recepcion[k%tam*tam+i];
				
			for (int i=0; i<tam; i++)
				columnak[i] = buf_recepcion[i*tam+k%tam];
		}
		
		MPI_Bcast(filak, tam, MPI_INT, k%tam, comm_vertical);
		MPI_Bcast(columnak, tam, MPI_INT, k%tam, comm_horizontal);
		
		for(i=0;i<nverts;i++)
			for(j=0;j<nverts;j++)
				if (i!=j && i!=k && j!=k){
					vikj=G.arista(i,k)+G.arista(k,j);
					vikj=min(vikj,G.arista(i,j));
					G.inserta_arista(i,j,vikj);   
				}
	}

	t=MPI_Wtime()-t;
	MPI_Finalize();


	cout << endl<<"EL Grafo con las distancias de los caminos más cortos es:" << endl;
	G.imprime();
	cout<< "Tiempo gastado= " << t << endl;


}



