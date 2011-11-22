#include <iostream>
#include <vector>
#include <algorithm>
#include <mpi.h>

using namespace std;

int main(int argc, char *argv[]){
    int rank, size;
    vector<int> v;
    vector<int> *subv;
    int tam = 100;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if(rank==0){
        for(int i=0; i<tam; i++)
            v.push_back(rand()%1000);
    }

    subv = new vector<int>(tam/size);
    MPI_Scatter(&v[0],tam/size,MPI_INT,&((*subv)[0]),tam/size,MPI_INT,0,MPI_COMM_WORLD);
    sort(subv->begin(), subv->end());

    vector<int> *ordenado;
    MPI_Status status;
    int paso = 1;

    while(paso<size){
        if(rank%(2*paso)==0){
            if(rank+paso<size){
            	vector<int> localVecino(subv->size());
            	ordenado = new vector<int>(subv->size()*2);

                MPI_Recv(&localVecino[0],localVecino.size(),MPI_INT,rank+paso,0,MPI_COMM_WORLD,&status);
                merge(subv->begin(),subv->end(),localVecino.begin(),localVecino.end(),ordenado->begin() );

                delete subv;
                subv = ordenado;
                ordenado = NULL;
            }
        }
        else{
            int vecino = rank-paso;
            MPI_Send(&((*subv)[0]),subv->size(),MPI_INT,vecino,0,MPI_COMM_WORLD);
            break;
        }
        paso = paso*2;
    }

    if(rank == 0){
    	cout<<endl<<"[";
    	for(unsigned int i = 0; i<subv->size();++i)
    		cout<< (*subv)[i]<<" , ";
    	cout<<"]"<<endl;
    }

    MPI_Finalize();
    return 0;

}
