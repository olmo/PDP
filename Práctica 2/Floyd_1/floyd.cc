#include <iostream>
#include <fstream>
#include <string.h>
#include <cstdlib>
#include "Graph.h"
#include "mpi.h"
 
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
	
	double t=MPI_Wtime();
	
	MPI_Bcast(&nverts, 1, MPI_INT, 0, MPI_COMM_WORLD);
	
	int comienzo = 0;
	int partes = nverts / size;
	int aux = nverts%size;
	
	if(aux>rank){
		partes++;
		comienzo = rank*partes;
	}
	else{
		comienzo = rank*partes+aux;
	}
	
	//Repartimos la matriz entre los procesos
	int *particion = new int[nverts*partes];
	MPI_Scatter(grafo, partes*nverts, MPI_INT, particion, partes*nverts, MPI_INT, 0, MPI_COMM_WORLD);
	
	int *filak = new int[nverts];
	int vijk;
	
	for(int k=0; k<nverts; k++){
		//Si el proceso actual tiene la fila k la difunde a los demás procesos
		if (rank == k/partes)
		  for (int i=0; i<nverts; i++)
			filak[i] = particion[(k%partes)*nverts+i];
			
		MPI_Bcast(filak, nverts, MPI_INT, k/partes, MPI_COMM_WORLD);
		
		for(int i=0; i<partes && i<nverts; i++){
			for (int j=0; j<nverts; j++){
				if (comienzo+i!=j && comienzo+i!=k && j!=k){
					vijk = particion[i*nverts+k] + filak[j];
					vijk = min(vijk,particion[i*nverts+j]);
					particion[i*nverts+j] = vijk;
				}
		    }
		}
	}
	
	MPI_Gather(particion, partes*nverts, MPI_INT, grafo, partes*nverts, MPI_INT, 0, MPI_COMM_WORLD);
	
	t=MPI_Wtime()-t;
	
	if(rank==0){
		cout << endl<<"EL Grafo con las distancias de los caminos más cortos es:"<<endl<<endl;
		I->A = grafo;
		I->imprime();
		cout<< "\nTiempo gastado = " << t << endl;
	}
 
	MPI_Finalize();
}

