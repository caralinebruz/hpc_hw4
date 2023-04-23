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
  MPI_Comm comm = MPI_COMM_WORLD;

  /* get name of host running MPI process */
  char processor_name[MPI_MAX_PROCESSOR_NAME];
  int name_len;
  MPI_Get_processor_name(processor_name, &name_len);
  printf("Rank %d/%d running on %s.\n", rank, world_size, processor_name);

  // read N from inputs
  long N = 20;
  sscanf(argv[1], "%d", &N);

  // set up the data
  //long* A = (long*) malloc(N * sizeof(long));
  //long* B1 = (long*) malloc(N * sizeof(long));

  // for (long i = 0; i < N; i++) A[i] = rand();
  //for (long i = 0; i < N; i++) A[i] = i;
  //for (long i = 0; i < N; i++) B1[i] = 0;


  int length_each = N/world_size;
  printf("length each: %d \n", length_each);
  
  /* timing */
  MPI_Barrier(comm);
  double tt = MPI_Wtime();


  // split up the array into world_size sections

  if (rank == 0) {

	  // create a array
	  int* A = (int*) malloc(N * sizeof(int));

	  printf("Array A values:\n");
	  for (int i = 0; i < N; i++) {
		  A[i] = rand() % 10 + 1;
		  printf("A[%d] = %d \n", i, A[i]);
	  }

	  // send each section to the other ranks
	  // method 1, send sequentially
	  for (int process=1; process<world_size; process++) {

	 	printf("process %d gets indices: \n", process);
	        int* subarray = (int*) malloc(length_each * sizeof(int));
		for (int i = 0; i < length_each; i++) {
			subarray[i] = A[length_each*process + i];	
			printf("\t %lu\n", subarray[i]);
		}

		// send it to the correct process
		int send_to_rank = process;
		MPI_Send(subarray, length_each, MPI_INT, send_to_rank, send_to_rank, MPI_COMM_WORLD);
		free(subarray);
	  }


	  for (int q=0; q<length_each; q++) {
	  	printf("Rank 0, process number A[%d]: %d \n", q, A[q]);
	  }


	  // do your own work
	  int offset = 0;
	  int partial_sum = 0;
	  
	  int* mysums = (int*) malloc(length_each * sizeof(int));
	  mysums[0] = 0;

	  for (int y=1; y<length_each+1; y++){

		//	printf("A[%d-1] \n",y, A[y-1]); 
		// 	printf(" %d + %d \n", mysums[y-1], A[y-1]);

		mysums[y] = mysums[y-1] + A[y-1];
		// printf(" rank 0 mysums[%d] = %d \n", y, mysums[y]);
		offset = mysums[y];
	  }
	  printf("  rank 0 offset %d \n", offset);

	  // then wait for them to respond
  }
  else {
	// receive your chunk
	int receive_from_rank = 0;
	MPI_Status status;

	int* sub = (int*) malloc(length_each * sizeof(int));

	MPI_Recv(sub, length_each, MPI_INT, receive_from_rank, rank, MPI_COMM_WORLD, &status);

	int offset = 0;
	int* mysums = (int*) malloc(length_each * sizeof(int));
	mysums[0] = 0;

	for (int y=1; y<length_each+1; y++) {
		mysums[y] = mysums[y-1] + sub[y-1];
		offset = mysums[y];
		printf("Rank %d, processing subvalue %d \n", rank, sub[y-1]);	
	}

	printf(" rank %d offset: %d\n", rank, offset);

	// send it back

	free(sub);
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
