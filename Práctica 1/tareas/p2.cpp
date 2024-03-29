#include <iostream>
#include <math.h> 
#include <cstdlib>
#include <mpi.h>
using namespace std;
 
int main(int argc, char *argv[]) 
{ 
    int n, rank, size;
    double PI25DT = 3.141592653589793238462643;
    double pi_local, pi, h, sum;
	
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
 
	if (rank == 0) {
		cout << "Introduce la precision del calculo (n > 0): ";
		cin >> n;
	}

	//Se transmite la precisión a todos los procesos desde el proceso 0
	MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
	
	if (n <= 0){
		MPI_Finalize();
		exit(0);
	} else {
		//Calculamos cuantos bloques tiene que calcular cada proceso
		int comienzo = 0;
		int part = n/size;
		int aux = n%size;
		
		//Si el modulo es mayor que 0 se van repartiendo las partes sobrantes entre los procesos
		//comenzando por el 0.
		if(aux>rank){
			part++;
			comienzo = rank*part;
		}
		else{
			comienzo = rank*part+aux;
		}
		
		//Cada proceso calcula solo los bloques que le corresponden
		h = 1.0 / (double) n;
		sum = 0.0;
		for (int i = comienzo+1; i <= comienzo+part; i ++) {
			double x = h * ((double)i - 0.5);
			sum += (4.0 / (1.0 + x*x));
		}
		pi_local = h * sum;
 
 		//Sumamos en la variable pi de cada proceso el valor parcial calculado por cada proceso
 		//en la variable pi_local
		MPI_Allreduce(&pi_local, &pi, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
 		
 		cout << "Soy el proceso " << rank << "y el valor aproximado de PI es: " << pi
			 << ", con un error de " << fabs(pi - PI25DT) << endl;
	}
 
    MPI_Finalize(); 
    return 0; 
}
