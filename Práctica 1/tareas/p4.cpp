#include <iostream>
#include <vector>
#include <cstdlib>
#include <mpi.h>
using namespace std;
 
int main(int argc, char *argv[]) {
    int rank, size;
 
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
 
    MPI_Comm comm_nuevo, comm_inverso;
    int rank_inverso, size_inverso;
    int rank_nuevo, size_nuevo;
    int a, b, dato=0;
    vector<int> Vector;
 
 	//Inicializamos los valores de las variables que se van a transmitir.
    if (rank == 0) {
        a = 2000;
        b = 1;
    } else {
        a = 0;
        b = 0;
    }
    
    //El primer proceso impar inicializa el vector que enviará los datos a los procesos impares.
    if(rank==1){
    	Vector.resize(size/2, 0);
    	for(int i=0; i<size/2; i++)
    		Vector[i] = (rand()%1000);
    }
 
 	//color==0 indica que el proceso es par, y color==1 que es impar
    int color = rank % 2;
    
    //Se crean los dos nuevos comunicadores
    //El primero estará dividido en dos, una parte con los procesos pares y otra con los impares.
    MPI_Comm_split(MPI_COMM_WORLD, color, rank, &comm_nuevo);
    MPI_Comm_split(MPI_COMM_WORLD, 0, -rank, &comm_inverso);
 
 	//Obtenemos el nuevo identificador de cada proceso en cada comunicador
    MPI_Comm_rank(comm_nuevo, &rank_nuevo);
    MPI_Comm_size(comm_nuevo, &size_nuevo);
    MPI_Comm_rank(comm_inverso, &rank_inverso);
    MPI_Comm_size(comm_inverso, &size_inverso);
 
 	//Transmitimos el valor de b a todos los procesos del comunicador con los procesos invertidos
    MPI_Bcast(&b, 1, MPI_INT, size-1, comm_inverso);
    //Transmitimos a solo a los procesos pares, ya que el proceso 0 pertenece al comunicador
    //con color 0 (pares)
    MPI_Bcast(&a, 1, MPI_INT, 0, comm_nuevo);
	
	
	//Si el proceso es impar enviamos el dato correspondiente desde el proceso 0 del comunicador
	//de los impares
	if(rank%2==1)
    	MPI_Scatter(&Vector[0], 1, MPI_INT, &dato, 1, MPI_INT, 0, comm_nuevo);
    
 	cout << "Soy el proceso " << rank << " de " << size << " dentro de MPI_COMM_WORLD," << "\n\t mi rango en COMM_nuevo es " << rank_nuevo << ", de " << size_nuevo  << ", el valor de a es " << a << ",\n\ten COMM_inverso mi rango es " << rank_inverso << " de " << size_inverso << " el valor de b es " << b <<"\n"<< "\ty el dato del vector si soy impar es " << dato << endl;
 
    MPI_Finalize();
    return 0;
}
