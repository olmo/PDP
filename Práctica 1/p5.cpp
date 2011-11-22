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

    long *vlocal, *y;
    double tiempo_inicio, tiempo_fin;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	A = new long *[size];//reservamos espacio para las n filas de la matriz.
	x = new long [size];//reservamos espacio para el vector.

 	if(rank==0){
        A[0] = new long [size * size];
        for (unsigned int i = 1; i < size; i++) {
            A[i] = A[i - 1] + size;
        }

        y = new long[size];

        // Rellena A y x con valores aleatorios
        srand(time(0));
        cout << "La matriz y el vector generados son " << endl;
        for (unsigned int i = 0; i < size; i++) {
            for (unsigned int j = 0; j < size; j++) {
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

        comprueba = new long [size];
        //Calculamos la multiplicacion secuencial para
        //despues comprobar que es correcta la solucion.
        for (unsigned int i = 0; i < size; i++) {
            comprueba[i] = 0;
            for (unsigned int j = 0; j < size; j++)
                comprueba[i] += A[i][j] * x[j];
        }

 	}

    vlocal = new long[size];

    MPI_Scatter(A[0], size, MPI_LONG, vlocal, size, MPI_LONG, 0, MPI_COMM_WORLD);
    MPI_Bcast(x, size, MPI_LONG, 0, MPI_COMM_WORLD);

    MPI_Barrier(MPI_COMM_WORLD);
    tiempo_inicio = MPI_Wtime();

    long subFinal = 0;
    for (unsigned int i = 0; i < size; i++) {
        subFinal += vlocal[i] * x[i];
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
        delete [] A[0];

        cout << "Tiempo: " << tiempo_fin-tiempo_inicio << "segundos.\n";
    }

    delete [] x;
    delete [] A;
    delete [] vlocal;


    return 0;

}
