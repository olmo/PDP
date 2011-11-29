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
	int size, rank, rank_hor, rank_ver, rank_cart;
	int *buf_envio, *buf_recepcion;
	int nverts, tam;
	Graph G;
	int *grafo, *temp;
	
	MPI_Init(&argc,&argv);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	if (argc != 2) {
		cerr << "Sintaxis: " << argv[0] << " <archivo de grafo>" << endl;
		MPI_Finalize();	
		return(-1);
	}
	
	int raiz_p = sqrt(size);

	if(rank==0){
		G.lee(argv[1]);
		cout << "EL Grafo de entrada es:"<<endl;
		G.imprime();
		nverts = G.vertices;
		grafo = G.A;

		if(nverts % raiz_p != 0){
			cerr << "Número de vertices no múltiplo de la raíz del número de procesos " << endl;
			MPI_Finalize();
			return (-1);
		}
		
		//La submatriz de cada proceso será de tam*tam
		tam = nverts/raiz_p;
	
		buf_envio = new int[nverts*nverts];
		temp = new int[nverts*nverts];
		
		MPI_Datatype bloque;
		MPI_Type_vector(tam, tam, nverts, MPI_INT, &bloque);
		MPI_Type_commit(&bloque);

		//Empaquetamos los datos a enviar
		int posicion=0, fila_p, columna_p, comienzo;
		for (int i=0; i<size; i++){
			fila_p = i/raiz_p;
			columna_p = i%raiz_p;
			comienzo = (columna_p*tam)+(fila_p*tam*tam*raiz_p);
			
			MPI_Pack(&grafo[comienzo], 1, bloque, buf_envio, sizeof(int)*nverts*nverts, &posicion,  MPI_COMM_WORLD);
		}
		
		MPI_Type_free(&bloque);
	}
	
	//Enviamos a todos los procesos el nº de vértices y tam
	MPI_Bcast(&tam, 1, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Bcast(&nverts, 1, MPI_INT, 0, MPI_COMM_WORLD);
	
	//Distribuimos la matriz entre los procesadores
	buf_recepcion = new int[tam*tam];
	MPI_Scatter(buf_envio, tam*tam, MPI_INT, buf_recepcion, tam*tam, MPI_INT, 0, MPI_COMM_WORLD);
	
	//Creamos un comunicador por cada fila y columna, y un comunicador cartesiano
	int dims[] = {raiz_p, raiz_p};
	int periods[] = {0, 0};
	int coord[2];
    
	MPI_Comm comm_horizontal, comm_vertical, COMM_CART;
	MPI_Comm_split(MPI_COMM_WORLD, rank/raiz_p, 0, &comm_horizontal);
	MPI_Comm_split(MPI_COMM_WORLD, rank%raiz_p, 0, &comm_vertical);
	MPI_Cart_create(MPI_COMM_WORLD , 2, dims, periods, true, &COMM_CART);
	MPI_Comm_rank(comm_horizontal, &rank_hor);
	MPI_Comm_rank(comm_vertical, &rank_ver);
	MPI_Comm_rank(COMM_CART, &rank_cart);

	// Bucle principal del algoritmo
	int *filak = new int[tam];
	int *columnak = new int[tam];
	int i,j,k,vikj, aux;
	
	int fila_f = rank/raiz_p*tam;
	int columna_f = rank%raiz_p*tam;
	
	double t = MPI_Wtime();
	
	for(k=0;k<nverts;k++){
		//Repartimos las filask entre los procesos correspondientes
		for(i=0; i<raiz_p; i++){
			coord[0] = k/tam; coord[1] = i;
			MPI_Cart_rank(COMM_CART, coord, &aux);
			
			if(rank_cart==aux){
				for (int l=0; l<tam; l++)
					filak[l] = buf_recepcion[k%tam*tam+l];
			}
			MPI_Bcast(filak, tam, MPI_INT, k/tam, comm_vertical);
		}
		
		//Repartimos las columnask entre los procesos correspondientes
		for(i=0; i<raiz_p; i++){
			coord[0] = i; coord[1] = k/tam;
			MPI_Cart_rank(COMM_CART, coord, &aux);
		
			if(rank_cart==aux){
				for (int l=0; l<tam; l++)
					columnak[l] = buf_recepcion[k%tam+l*tam];
			}
			MPI_Bcast(columnak, tam, MPI_INT, k/tam, comm_horizontal);
		}
		
		for(i=0;i<tam;i++)
			for(j=0;j<tam;j++)
				if (i+fila_f!=j+columna_f && i+fila_f!=k && j+columna_f!=k){
					vikj = columnak[i]+filak[j];
					vikj = min(vikj, buf_recepcion[i*tam+j]);
					buf_recepcion[i*tam+j] = vikj;
				}
	}
	
	t = MPI_Wtime()-t;
	
	//Obtenemos en el proceso 0 todas las submatrices
	MPI_Gather(buf_recepcion, tam*tam, MPI_INT, temp, tam*tam, MPI_INT, 0, MPI_COMM_WORLD);

	if(rank==0){
		//Desempaquetamos los datos para obtener la matriz final
		MPI_Datatype bloque;
		MPI_Type_vector(tam, tam, nverts, MPI_INT, &bloque);
		MPI_Type_commit(&bloque);
		int *solucion = new int[nverts*nverts];
		int posicion=0, fila_p, columna_p, comienzo;
		
		for (int i=0; i<size; i++){
			fila_p = i/raiz_p;
			columna_p = i%raiz_p;
			comienzo = (columna_p*tam)+(fila_p*tam*tam*raiz_p);
		
			MPI_Unpack(temp, sizeof(int)*nverts*nverts, &posicion, &solucion[comienzo], 1, bloque, MPI_COMM_WORLD);
		}
		
		MPI_Type_free(&bloque);
		G.A = solucion;
	
		cout << endl<<"EL Grafo con las distancias de los caminos más cortos es:" << endl;
		G.imprime();
		cout<< "Tiempo gastado= " << t << endl << endl;
	}
	
	MPI_Finalize();
}

