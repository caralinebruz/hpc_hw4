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
  printf("Will run %d repeats\n", N);

  int rank, size;
  MPI_Comm_rank(comm, &rank);
  MPI_Comm_size(comm, &size);

  // define the first message out
  int message_out = 12;
  int message_in;

    // get hostname of process, for testing only
    int mpisize;
    char processor_name[MPI_MAX_PROCESSOR_NAME];
    int name_len;
    MPI_Get_processor_name(processor_name, &name_len);
    printf("Rank %d/%d running on %s.\n", rank, size, processor_name);
    // end of get hostname
    
    MPI_Barrier(comm);    
    for (int n=0; n<N; n++) {

      if (rank == 0 ) {
    	// send the first one
	message_out = message_out + rank;
	int send_to_rank = 1;
	MPI_Send(&message_out, 1, MPI_INT, send_to_rank, 999, MPI_COMM_WORLD);
    
    	// then, receive it from previous 
	int message_recv;
	int receive_from_rank = size - 1;
	MPI_Status status;
	MPI_Recv(&message_recv, 1, MPI_INT, receive_from_rank, 999, MPI_COMM_WORLD, &status);
    	printf("Iter:%d, Rank %d, Received message is %d\n", n, rank, message_recv);
	message_out = message_recv;
      }
      else {
      
      
      	if ( rank < (size - 1) ) {
	 // regular iteration, pass the value to the next one
	 int message_recv;
	 int receive_from_rank = rank - 1;
	 MPI_Status status;
         MPI_Recv(&message_recv, 1, MPI_INT, receive_from_rank, 999, MPI_COMM_WORLD, &status);
         printf("Iter:%d, Rank %d, Received message is %d\n", n, rank, message_recv);

	 // send 
	 message_out = message_recv + rank;
	 int send_to_rank = rank + 1;
	 MPI_Send(&message_out, 1, MPI_INT, send_to_rank, 999, MPI_COMM_WORLD);
        } 

       else if ( rank == (size - 1) ) {
   	 // this is the final one, it sends back to rank 0
	 int message_recv;
         int receive_from_rank = rank - 1;
         MPI_Status status;
         MPI_Recv(&message_recv, 1, MPI_INT, receive_from_rank, 999, MPI_COMM_WORLD, &status);
         printf("Iter:%d, Rank %d, Received message is %d\n", n, rank, message_recv); 
	
	
	 // send to 0
         message_out = message_recv + rank;
         int send_to_rank = 0;
         MPI_Send(&message_out, 1, MPI_INT, send_to_rank, 999, MPI_COMM_WORLD);	
       }

      } 
    

    } // corresponds to N loops

   MPI_Barrier(comm);
  if (!rank) {
  	printf("result: %d \n", message_out);
  }

  MPI_Finalize();
  return 0;
}
