#include <iostream>
#include <cstdlib>
#include <ctime>
#include <mpi.h>
using namespace std;

int main(int argc, char * argv[]) {
    int rank, size;
    long **A, // Matriz a multiplicar
    *x, // Vector que vamos a multiplicar
    *comprueba; // Guarda el resultado final (calculado secuencialmente), su valor

    long **vlocal, *y;
    double tiempo_inicio, tiempo_fin;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    
    if(argc < 2){
		cerr << "El formato de ejecución es : filas columnas\n";
		MPI_Finalize();
		exit(0);
	}

	int columnas = atoi(argv[1]);
	int filas = size;
	
	cout << filas;
    
    if(rank==0){
		A = new long *[filas];//reservamos espacio para las n filas de la matriz.
		for(int i=0; i<filas; i++)
			A[i] = new long[columnas];
		
		srand(time(0));
		for(int i=0; i<filas; i++){
			for(int j=0; j<columnas; j++){
			if (j == 0) cout << "[";
                A[i][j] = rand() % 1000;
                cout << A[i][j];
                if (j == size - 1) cout << "]";
                else cout << "  ";
            }
            x[i] = rand() % 100;
            cout << "\t  [" << x[i] << "]" << endl;
		}
		cout << "\n";
		
		comprueba = new long [columnas];
        //Calculamos la multiplicacion secuencial para
        //despues comprobar que es correcta la solucion.
        for (unsigned int i = 0; i < filas; i++) {
            comprueba[i] = 0;
            for (unsigned int j = 0; j < columnas; j++)
                comprueba[i] += A[i][j] * x[j];
        }
	}

	x = new long [columnas];//reservamos espacio para el vector.
	
	int partes = filas/size;
	int aux = filas%size;
	int comienzo = 0;
	
	if(aux>rank){
		partes++;
		comienzo = rank*partes;
	}
	else{
		comienzo = rank*partes+aux;
	}
	

    vlocal = new long*[partes];
    for(int i=0; i<partes; i++)
    	vlocal[i] = new long[columnas];
    
	for(int i=comienzo; i<comienzo+partes; i++)
    	MPI_Scatter(A[i], columnas, MPI_LONG, vlocal[i-comienzo], columnas, MPI_LONG, 0, MPI_COMM_WORLD);
    
    MPI_Bcast(x, columnas, MPI_LONG, 0, MPI_COMM_WORLD);

    MPI_Barrier(MPI_COMM_WORLD);
    tiempo_inicio = MPI_Wtime();

    long subFinal = 0;
    for (int i = 0; i<partes; i++) {
    	for(int j=0; j<columnas; j++)
        	subFinal += vlocal[i][j] * x[j];
    }

    MPI_Barrier(MPI_COMM_WORLD);
    tiempo_fin = MPI_Wtime();

    MPI_Gather(&subFinal, 1, MPI_LONG, y, 1, MPI_LONG, 0, MPI_COMM_WORLD);

    MPI_Finalize();

    if(rank==0){
        cout << "El resultado obtenido y el esperado son:" << endl;
        for (unsigned int i = 0; i < size; i++) {
            cout << "\t" << y[i] << "\t|\t" << comprueba[i] << endl;
        }

        delete [] y;
        delete [] comprueba;
        for(int i=0; i<filas; i++)
        	delete A[i];
        delete [] A;

        cout << "Tiempo: " << tiempo_fin-tiempo_inicio << "segundos.\n";
    }

    delete [] x;
    for(int i=0; i<partes; i++)
    	delete vlocal[i];
    delete [] vlocal;


    return 0;

}
