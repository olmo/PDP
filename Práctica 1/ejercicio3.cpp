#include <math.h>
#include <cstdlib>
#include <iostream>
using namespace std;
 
int main(int argc, char *argv[]) 
{ 
 	int rank, size;
 	int n;
    MPI_Status estado;
 
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    
    if(rank==0){
    	cout<<"introduce la precision del calculo (n > 0): ";
		cin>>n;
		
		
    }
    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
    
    int partes = n/size;
    
	
	double PI25DT = 3.141592653589793238462643;
	double h = 1.0 / (double) n;
	double sum = 0.0;
	for (int i = partes*rank; i <= partes*rank+partes; i++) {
		double x = h * ((double)i - 0.5);
		sum += (4.0 / (1.0 + x*x));
	}
	double pi = sum * h;
	cout << "El valor aproximado de PI es: " << pi << ", con un error de " << fabs(pi -
PI25DT) << endl;
	return 0;
 
}
