#include <iostream>
#include <fstream>
#include <string.h>
#include <cstdlib>
#include "Graph.h"
#include "mpi.h"

#define min(a,b) ((a) < (b)) ? (a) : (b)
 
using namespace std;
 
int main (int argc, char *argv[]){
	int rank, size;
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    int nverts;
    int *grafo;
    Graph *I;
 
	if (argc != 2){
		cerr << "Sintaxis: " << argv[0] << " <archivo de grafo>" << endl;
		exit(-1);
	}
	
	Graph G;
	
	if(rank==0){
		G.lee(argv[1]);
		cout << "EL Grafo de entrada es:"<<endl;
		G.imprime();
		nverts=G.vertices;
		I=&G;
		grafo = G.A;
		
	}
	
	MPI_Bcast(&nverts, 1, MPI_INT, 0, MPI_COMM_WORLD);
	
	int partes = nverts / size;
	if (nverts % size)
		partes++;
	
	//Repartimos la matriz entre los procesos
	int *particion = new int[nverts*partes];
	MPI_Scatter(grafo, partes*nverts, MPI_INT, particion, partes*nverts, MPI_INT, 0, MPI_COMM_WORLD);
 
	// BUCLE PPAL DEL ALGORITMO
	/*int i,j,k,vikj;
	for(k=0;k<nverts;k++){
		for(i=0;i<nverts;i++)
			for(j=0;j<nverts;j++)
				if (i!=j && i!=k && j!=k){
					vikj=G.arista(i,k)+G.arista(k,j);
					vikj=min(vikj,G.arista(i,j));
					I->inserta_arista(i,j,vikj);
				}
		G=*I;
	}*/
	
	int *filak = new int[nverts];
	int vijk;
	
	for(int k=0; k<nverts; k++){
		//Si el proceso actual tiene la fila k la difunde a los demás procesos
		if (rank == k/partes)
		  for (int i=0; i<nverts; i++)
			filak[i] = particion[(k%partes)*nverts+i];
			
		MPI_Bcast(filak, nverts, MPI_INT, k/partes, MPI_COMM_WORLD);
	
		for(int i=0; i<partes && i<nverts; i++){
			if (particion[i*nverts+k] < 1)
				continue;
			for (int j = 0; j < nverts; j++){
				if (filak[j] < 1)
				  continue;
				if (particion[i*nverts+j] < 0)
				  vijk = particion[i*nverts+k] + filak[j];
				else
				  vijk = min(particion[i*nverts+j],particion[i*nverts+k]+filak[j]);
				  
				particion[i*nverts+j] = vijk;
		    }
		}
	}
	
	MPI_Gather(particion, partes*nverts, MPI_INT, grafo, partes*nverts, MPI_INT, 0, MPI_COMM_WORLD);
	
	if(rank==0){
		cout << endl<<"EL Grafo con las distancias de los caminos más cortos es:"<<endl<<endl;
		I->A = grafo;
		I->imprime();
	}
 
	MPI_Finalize();
}

