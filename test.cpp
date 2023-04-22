#include <stdio.h>
#include <cstdlib>
#include <mpi.h>
#include <iostream>

using namespace std;

int main(int argc, char* argv[])
{

    MPI_Init(&argc, &argv);
    if (argc < 2)
    {
        printf("please give the numebr of iterations you want to test!\n");
        abort();
    }
    int N = atoi(argv[1]);
    int p;//number of nodes/processes we have.
    MPI_Comm_size(MPI_COMM_WORLD, &p);
    int rank;
    MPI_Comm comm = MPI_COMM_WORLD;
    MPI_Comm_rank(comm, &rank);
    int inte = 0;
    MPI_Barrier(comm);
    double tt = MPI_Wtime();
    for (int k = 0;k < N;k++) {
        MPI_Status status;
        if (rank == 0) {//first node just send and then receive
            MPI_Send(&inte, 1, MPI_INT, 1, k, comm);
            MPI_Recv(&inte, 1, MPI_INT, p - 1, k, comm, &status);
        }
        else if (rank == p - 1) {//last node receive and then send to first
            MPI_Recv(&inte, 1, MPI_INT, p - 2, k, comm, &status);
            inte += rank;
            MPI_Send(&inte, 1, MPI_INT, 0, k, comm);
        }
        else {//interior node receive and then send to next
            MPI_Recv(&inte, 1, MPI_INT, rank - 1, k, comm, &status);
            inte += rank;
            MPI_Send(&inte, 1, MPI_INT, rank + 1, k, comm);
        }

    }
    MPI_Barrier(comm);
    tt = MPI_Wtime() - tt;
    if (!rank) printf("result is: %d,result should be: %d,estimated latency: %e ms\n", inte, p * (p - 1) * N / 2, tt / (N * p) * 1000);
    MPI_Finalize();
}
