#include <iostream>
#include <fstream>
#include <string.h>
#include <cstdlib>
#include "Graph.h"
#include "mpi.h"
 
using namespace std;
 
//**************************************************************************
 
int main (int argc, char *argv[]){
	int rank, size;
	MPI::Init(argc,argv);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    int nverts;
 
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
	}
	
	MPI_Bcast(&nverts, 1, MPI_INT, 0, MPI_COMM_WORLD);
 
	Graph *I=&G;
	
	int partes = nverts / size;
	if (nverts % size)
		partes++;
 
	// BUCLE PPAL DEL ALGORITMO
	int i,j,k,vikj;
	for(k=0;k<nverts;k++){
		for(i=0;i<nverts;i++)
			for(j=0;j<nverts;j++)
				if (i!=j && i!=k && j!=k){
					vikj=G.arista(i,k)+G.arista(k,j);
					vikj=min(vikj,G.arista(i,j));
					I->inserta_arista(i,j,vikj);   
				}
		G=*I;
	}
 
	MPI::Finalize();

	cout << endl<<"EL Grafo con las distancias de los caminos más cortos es:"<<endl<<endl;
	I->imprime();
}

