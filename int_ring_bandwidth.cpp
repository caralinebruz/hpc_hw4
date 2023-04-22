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
  long size_array = 2000000;
  char* message = (char*) malloc(size_array);
  for (long i = 0; i < N; i++) message[i] = 42;

    // get hostname of process, for testing only
    char processor_name[MPI_MAX_PROCESSOR_NAME];
    int name_len;
    MPI_Get_processor_name(processor_name, &name_len);
    printf("Rank %d/%d running on %s.\n", rank, size, processor_name);
    // end of get hostname
    
    MPI_Barrier(comm);  
    double tt = MPI_Wtime();  
    for (int n=0; n<N; n++) {

      if (rank == 0 ) {
    	// send the first one
	int send_to_rank = 1;
	MPI_Send(message, size_array, MPI_CHAR, send_to_rank, 999, MPI_COMM_WORLD);
    
    	// then, receive it from previous 
	int receive_from_rank = size - 1;
	MPI_Status status;
	MPI_Recv(message, size_array, MPI_CHAR, receive_from_rank, 999, MPI_COMM_WORLD, &status);
    	// printf("Iter:%d, Rank %d, Received message is %d\n", n, rank, message_recv);
      }
      else {
      
      
      	if ( rank < (size - 1) ) {
	 // regular iteration, pass the value to the next one
	 int receive_from_rank = rank - 1;
	 MPI_Status status;
         MPI_Recv(message, size_array, MPI_CHAR, receive_from_rank, 999, MPI_COMM_WORLD, &status);
         // printf("Iter:%d, Rank %d, Received message is %d\n", n, rank, message_recv);

	 // send 
	 int send_to_rank = rank + 1;
	 MPI_Send(message, size_array, MPI_CHAR, send_to_rank, 999, MPI_COMM_WORLD);
        } 

       else if ( rank == (size - 1) ) {
   	 // this is the final one, it sends back to rank 0
         int receive_from_rank = rank - 1;
         MPI_Status status;
         MPI_Recv(message, size_array, MPI_CHAR, receive_from_rank, 999, MPI_COMM_WORLD, &status);
         // printf("Iter:%d, Rank %d, Received message is %d\n", n, rank, message_recv); 
	
	
	 // send to 0
         int send_to_rank = 0;
         MPI_Send(message, size_array, MPI_CHAR, send_to_rank, 999, MPI_COMM_WORLD);	
       }

      } 
    

    } // corresponds to N loops


   //MPI_Barrier(comm);
   tt = MPI_Wtime() - tt;

   if (!rank) {
  	printf("Int ring bandwidth: %e GB/s \n", (size_array*N)/tt/1e9 * 1000);	   
   }

  MPI_Finalize();
  return 0;
}
