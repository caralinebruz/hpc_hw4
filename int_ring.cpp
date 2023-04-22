#include <stdlib.h>
#include <stdio.h>
#include <mpi.h>

int main(int argc, char** argv) {
  MPI_Init(&argc, &argv);
  MPI_Comm comm = MPI_COMM_WORLD;

  int rank, mpisize;
  MPI_Comm_rank(comm, &rank);
  MPI_Comm_size(comm, &mpisize);

  long N = 1;

  for (int n=0; n<N; n++) {


    // get hostname of process, for testing only
    int mpisize;
    char processor_name[MPI_MAX_PROCESSOR_NAME];
    int name_len;
    MPI_Get_processor_name(processor_name, &name_len);
    printf("Rank %d/%d running on %s.\n", rank, mpisize, processor_name);
    // end of get hostname

    // start with the first one
    if (rank == 0) {


        int message_out = 12;

        int tag = message_out;
        int send_to_rank = rank + 1;

        MPI_Send(&message_out, 1, MPI_INT, send_to_rank, 999, MPI_COMM_WORLD);


    }
    else {
        int message_in;
        MPI_Status status;
        int receive_from_rank = rank -1;

        MPI_Recv(&message_in, 1, MPI_INT, receive_from_rank, 999, MPI_COMM_WORLD, &status);
        printf("Rank %d, The message is %d\n", message_in, rank);
    }



  }


  MPI_Finalize();
  return 0;
}
