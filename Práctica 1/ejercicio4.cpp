#include <vector>
#include <cstdlib>
#include <iostream>
#include <mpi.h>
using namespace std;
 
int main(int argc, char *argv[]) {
    int tama;
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
            cout << "No se ha especificado numero de elementos, multiplo de la cantidad de entrada, por defecto sera " << size * 100;
            cout << "\nUso: <ejecutable> <cantidad>" << endl;
        }
        tama = size * 100;
    } else {
        tama = atoi(argv[1]);
        if (tama < size) tama = size;
        else {
            int i = 1, num = size;
            while (tama > num) {
                ++i;
                num = size*i;
            }
            if (tama != num) {
                if (rank == 0)
                    cout << "Cantidad cambiada a " << num << endl;
                tama = num;
            }
        }
    }
    
    VectorA.resize(tama, 0);
	VectorB.resize(tama, 0);
	if(rank==0){
		for (long i = 0; i < tama; ++i) {
		    VectorA[i] = i + 1; // Vector A recibe valores 1, 2, 3, ..., tama
		    VectorB[i] = (i + 1)*10; // Vector B recibe valores 10, 20, 30, ..., tama*10
		}
	}
	
    tam_parte = tama/size;
    PartA.resize(tam_parte,0);
    PartB.resize(tam_parte,0);
    
    MPI_Scatter(&VectorA[0], tam_parte, MPI_LONG, &PartA[0], tam_parte, MPI_LONG, 0, MPI_COMM_WORLD);
    MPI_Scatter(&VectorB[0], tam_parte, MPI_LONG, &PartB[0], tam_parte, MPI_LONG, 0, MPI_COMM_WORLD);
 
    // Calculo de la multiplicacion escalar entre vectores
    long total = 0;
    for (long i = 0; i < tam_parte ; ++i) {
        total += PartA[i] * PartB[i];
    }
    
    MPI_Reduce(&total, &total_global, 1, MPI_LONG, MPI_SUM, 0, MPI_COMM_WORLD);
 
 	if(rank==0)
    	cout << "Total = " << total_global << endl;
    
    MPI_Finalize();
    return 0;
}
