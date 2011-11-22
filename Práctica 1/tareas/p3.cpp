#include <vector>
#include <cstdlib>
#include <iostream>
#include <math.h>
#include <mpi.h>
using namespace std;
 
int main(int argc, char *argv[]) {
    int tam;
    int rank, size;
 	long total_global=0;
    MPI_Status estado;
    long tam_parte=0;
    vector<long> VectorA, VectorB, PartA, PartB;
	
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    
 	
 	if (argc < 2) {
        if (rank == 0) {
            cout << "No se ha especificado numero de elementos, por defecto sera " << size * 100;
            cout << "\nUso: <ejecutable> <cantidad>" << endl;
        }
        tam = size * 100;
    } else {
        tam = atoi(argv[1]);
    }
    
    VectorA.resize(tam, 0);
	
	//Si es el proceso 0 inicializa el VectorA
	if(rank==0){
		for (long i = 0; i < tam; ++i) {
		    VectorA[i] = i + 1;
		}
	}
	
	//Calculamos el tamaño de la parte redondeando si es necesario
	double prov = tam/size;
	ceil(prov);
	tam_parte = prov;
	
    PartA.resize(tam_parte,0);
    //Inicializamos la parte correspondiente del vector B
    VectorB.resize(tam_parte, 0);
    for (long i = 0; i < tam_parte; ++i) {
	    VectorB[i] = ((tam_parte*rank+i) + 1)*10;
	}
    
    //Enviamos a cada vector la parte correspondiente que tiene que calcular de VectorA
    MPI_Scatter(&VectorA[0], tam_parte, MPI_LONG, &PartA[0], tam_parte, MPI_LONG, 0, MPI_COMM_WORLD);
 
    // Calculamos la multiplicación escalar correspondiente
    long total = 0;
    for (long i = 0; i < tam_parte ; ++i) {
        total += PartA[i] * VectorB[i];
    }
    
    //Sumamos todas las partes en la variable total_global del proceso 0
    MPI_Reduce(&total, &total_global, 1, MPI_LONG, MPI_SUM, 0, MPI_COMM_WORLD);
 
 	if(rank==0)
    	cout << "Total = " << total_global << endl;
    
    MPI_Finalize();
    return 0;
}
