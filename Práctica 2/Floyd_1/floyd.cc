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
		MPI_Finalize();	
		exit(-1);
	}
	
	Graph G;
	
	if(rank==0){
		G.lee(argv[1]);
		nverts=G.vertices;
		
		if(nverts % size != 0){
			cerr << "\n\nNúmero de vertices no múltiplo de la raíz del número de procesos\n\n" << endl;
			MPI_Abort(MPI_COMM_WORLD, 1);
			return (-1);
		}
		
		cout << "EL Grafo de entrada es:"<<endl;
		G.imprime();
		
		I=&G;
		grafo = G.A;
	}
	
	//Transmitimos a todos los procesos el número de vértices
	MPI_Bcast(&nverts, 1, MPI_INT, 0, MPI_COMM_WORLD);
	
	int comienzo = 0;
	int partes = nverts / size;
	int aux = nverts%size;
	
	//Calculamos cuantas filas tiene que calcular cada proceso
	//y cual es la fila inicial de cada uno con respecto a la matriz total
	if(aux>rank){
		partes++;
		comienzo = rank*partes;
	}
	else{
		comienzo = rank*partes+aux;
	}
	
	//Repartimos la matriz entre los procesos
	//Cada proceso tendrá su submatriz "particion"
	int *particion = new int[nverts*partes];
	MPI_Scatter(grafo, partes*nverts, MPI_INT, particion, partes*nverts, MPI_INT, 0, MPI_COMM_WORLD);
	
	//Creamos el vector donde se almacenará la fila k
	int *filak = new int[nverts];
	int vijk;
	
	MPI_Barrier(MPI_COMM_WORLD);
	double t=MPI_Wtime();
	
	for(int k=0; k<nverts; k++){
		//Si el proceso actual tiene la fila k la difunde a los demás procesos
		if (rank == k/partes)
		  for (int i=0; i<nverts; i++)
			filak[i] = particion[(k%partes)*nverts+i];
			
		MPI_Bcast(filak, nverts, MPI_INT, k/partes, MPI_COMM_WORLD);
		
		//Cada proceso recorre solo las filas que tiene en su submatriz (particion)
		//"comienzo" indica la fila inicial de cada proceso con respecto a la matriz total
		for(int i=0; i<partes; i++){
			for (int j=0; j<nverts; j++){
				if (comienzo+i!=j && comienzo+i!=k && j!=k){
					vijk = particion[i*nverts+k] + filak[j];
					vijk = min(vijk,particion[i*nverts+j]);
					particion[i*nverts+j] = vijk;
				}
		    }
		}
	}
	
	t=MPI_Wtime()-t;
	
	//Obtenemos en el proceso 0 la información calculada por cada proceso
	MPI_Gather(particion, partes*nverts, MPI_INT, grafo, partes*nverts, MPI_INT, 0, MPI_COMM_WORLD);
	
	if(rank==0){
		cout << endl<<"EL Grafo con las distancias de los caminos más cortos es:"<<endl<<endl;
		I->A = grafo;
		I->imprime();
		cout<< "\nTiempo gastado = " << t << endl;
	}
 
	MPI_Finalize();
}

