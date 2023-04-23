#include <stdio.h>
#include <math.h>
#include <mpi.h>
#include <string.h>


int main(int argc, char * argv[]) {
  int rank, world_size;
  MPI_Status status, status1;

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &world_size);

  /* get name of host running MPI process */
  char processor_name[MPI_MAX_PROCESSOR_NAME];
  int name_len;
  MPI_Get_processor_name(processor_name, &name_len);
  printf("Rank %d/%d running on %s.\n", rank, world_size, processor_name);

  // read N from inputs
  long N = 20;
  sscanf(argv[1], "%d", &N);

  // set up the data
  long* A = (long*) malloc(N * sizeof(long));
  long* B1 = (long*) malloc(N * sizeof(long));

  // for (long i = 0; i < N; i++) A[i] = rand();
  for (long i = 0; i < N; i++) A[i] = i;
  for (long i = 0; i < N; i++) B1[i] = 0;

  /* timing */
  MPI_Barrier(MPI_COMM_WORLD);
  double tt = MPI_Wtime();


  // split up the array into world_size sections
  int length_each = N/world_size;
  printf("length each: %d \n");

  if (rank == 0) {

	  // send each section to the other ranks
	  for (int process=1; process<world_size; process++) {

		  // take a chunk
		  // from 5 ... 10
	 	printf("process %d gets indices: \n", process);
	        long* subarray = (long*) malloc(length_each * sizeof(long));
		for (long i = 0; i < length_each; i++) {
			subarray[i] = A[length_each*process + i];	
			printf("\t %lu\n", subarray[i]);
		}


		// send it to the correct process
		int send_to_rank = process;
		MPI_Send(subarray, length_each, MPI_INT, send_to_rank, 999, MPI_COMM_WORLD);
	  }
	  // do your own work
	  //
	  // then wait for them to respond
  }
  else {
	// receive your chunk
	int receive_from_rank = 0;
	MPI_Status status;

	long* sub = (long*) malloc(length_each * sizeof(long));
	MPI_Recv(sub, length_each, MPI_INT, receive_from_rank, 999, MPI_COMM_WORLD, &status);
	printf("Received from rank 0:\n");
	for (int j=0; j<length_each; j++) {

		printf("\t\t Rank %d received %d\n ", rank, sub[j]);
		
	}
	// do your work
	// send it back

  }






  /* timing */
  MPI_Barrier(MPI_COMM_WORLD);
  double elapsed = MPI_Wtime() - tt;
  if (0 == rank) {
    printf("Time elapsed is %f seconds.\n", elapsed);
  }

  // do some error checking late

  MPI_Finalize();
  return 0;
}
