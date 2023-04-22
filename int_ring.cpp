#include <stdlib.h>
#include <stdio.h>
#include <mpi.h>

int main(int argc, char** argv) {
  MPI_Init(&argc, &argv);
  MPI_Comm comm = MPI_COMM_WORLD;

  if (argc < 2) {
    printf("Usage: mpirun ./int_ring <n_repeats>\n");
    abort();
  }
  int N = atoi(argv[1]);


  int rank, size;
  MPI_Comm_rank(comm, &rank);
  MPI_Comm_size(comm, &size);


  for (int n=0; n<N; n++) {


    // get hostname of process, for testing only
    int mpisize;
    char processor_name[MPI_MAX_PROCESSOR_NAME];
    int name_len;
    MPI_Get_processor_name(processor_name, &name_len);
    printf("Rank %d/%d running on %s.\n", rank, size, processor_name);
    // end of get hostname

    // start with the first one
    if (rank == 0) {

	printf("I am Rank 0, will send to Rank 1 ...\n");
        int message_out = 12;

        int tag = message_out;
        int send_to_rank = (rank + 1) % size;

        MPI_Send(&message_out, 1, MPI_INT, send_to_rank, 999, MPI_COMM_WORLD);

	// next add the wait for the end of the loop
	int message_in;
	MPI_Status status;
	int receive_from_rank = rank -1;

    }
    else {
        int message_in;
        MPI_Status status;
        int receive_from_rank = rank -1;

        MPI_Recv(&message_in, 1, MPI_INT, receive_from_rank, 999, MPI_COMM_WORLD, &status);
        printf("Rank %d, The message is %d\n", rank, message_in);

	// if its not the final one
	// will stop when rank < (3-1)
	if (rank < (size - 1)) {
		
		int message_out = message_in + rank;
		int send_to_rank = rank + 1;
		MPI_Send(&message_out, 1, MPI_INT, send_to_rank, 999, MPI_COMM_WORLD);
    	}


    }
  }


  MPI_Finalize();
  return 0;
}
