#include <mpi.h>
#include <iostream>
using namespace std;
 
int main(int argc, char *argv[])
{
    int rank, size, contador=0;
    MPI_Status estado;
 
    MPI_Init(&argc, &argv); // Inicializamos la comunicacion de los procesos
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank); // Obtenemos el valor de nuestro identificador
    
    if(rank>0){
		MPI_Recv(&contador // Referencia al vector donde se almacenara lo recibido
		        ,1 // tamaño del vector a recibir
		        ,MPI_INT // Tipo de dato que recibe
		        ,rank-1 // pid del proceso origen de la que se recibe
		        ,0 // etiqueta
		        ,MPI_COMM_WORLD // Comunicador por el que se recibe
		        ,&estado); // estructura informativa del estado
		cout << "Soy el proceso "<< rank << " y he recibido " << contador << endl;
		contador++;
	}
 	
    if(rank<size-1){
		//Envia y recibe mensajes
		MPI_Send(&contador //referencia al vector de elementos a enviar
		        ,1 // tamaño del vector a enviar
		        ,MPI_INT // Tipo de dato que envias
		        ,rank+1 // pid del proceso destino
		        ,0 //etiqueta
		        ,MPI_COMM_WORLD); //Comunicador por el que se manda
    }
    
	
 
    MPI_Finalize();
    return 0;
}
