#include <mpi.h>
#include <iostream>
using namespace std;
 
int main(int argc, char *argv[])
{
    int rank, size, dato=0;
    MPI_Status estado;
 
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    
    //Comprobamos que el proceso sea mayor que 1, ya que el proceso 0 y 1 no reciben nada.
    if(rank>1){
    	//Se recibe el identificador de rank-2, que concidirá con el anterior proceso par o impar.
		MPI_Recv(&dato,1,MPI_INT,rank-2,0,MPI_COMM_WORLD,&estado);
		cout << "Soy el proceso "<< rank << " y he recibido " << dato << endl;
	}
 	
 	//Los dos ultimos procesos no envían sus identificadores
    if(rank<size-2){
    	//Se envía el identificador a rank+2, que condice con siguiente proceso par o impar.
		MPI_Send(&rank,1,MPI_INT,rank+2,0,MPI_COMM_WORLD);
	}
	
    MPI_Finalize();
    return 0;
}
