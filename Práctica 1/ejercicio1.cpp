#include <mpi.h>
#include <iostream>
using namespace std;
 
int main(int argc, char *argv[])
{
	MPI_Init(&argc, &argv);
	int size,rank;
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    cout<<"¡Hola Mundo soy el proceso " << rank << "de " << size << "que somos" <<endl;
 
 	MPI_Finalize();
    return 0;
}
