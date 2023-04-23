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
  int* mysums = (int*) malloc(length_each * sizeof(int));
  mysums[0] = 0;

  int offset = 0;


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


	  //for (int q=0; q<length_each; q++) {
	 // 	printf("Rank 0, process number A[%d]: %d \n", q, A[q]);
	  //}


	  // do your own work
	  
	  // int* mysums = (int*) malloc(length_each * sizeof(int));
	  // mysums[0] = 0;

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

	// int* mysums = (int*) malloc(length_each * sizeof(int));
	// mysums[0] = 0;

	for (int y=1; y<length_each+1; y++) {
		mysums[y] = mysums[y-1] + sub[y-1];
		offset = mysums[y];
		printf("Rank %d, processing subvalue %d \n", rank, sub[y-1]);	
	}

	printf(" rank %d offset: %d\n", rank, offset);

	free(sub);

	// send it back
	//
	

	// free(sub);
  }

  // when they are done with their subarray processing, then they can allgather
  // create a new buffer for send receive for the offsets

  int number_of_ints = 1;
  int* sendarray = (int*) malloc(number_of_ints * sizeof(int));
  sendarray[0] = offset;
  int* rbuf = (int*) malloc(world_size* number_of_ints * sizeof(int));

  MPI_Allgather( sendarray, number_of_ints, MPI_INT, rbuf, number_of_ints, MPI_INT, comm); 
  for (int s=0; s<world_size; s++) {
	  printf("Rank %d receive buffer [%d]\n", rank, rbuf[s]); 
  }

  // now that you have everyone's offset, find the total offset you should add to your onw
  // by summing offsets of processors with a lower rank than you
  int total_offset_to_add = 0;
  for (int t=0; t<world_size; t++) {
  
  	if (t < rank) {
		total_offset_to_add += rbuf[t];
	}
  }
  printf("Rank %d total offset to add: %d \n", rank, total_offset_to_add);


  // add this total offset to your sums array
  for (int u=0; u<length_each; u++) {
	int mysums_prev = mysums[u];
  	mysums[u] += total_offset_to_add;
	printf("rank %d previous val: %d new val: %d \n", rank, mysums_prev, mysums[u]);
  }

  // barrier here to wait for everyone
  MPI_Barrier(comm);


  // gather everyones final arrays
  int* sendarray_final = (int*) malloc(length_each * sizeof(int));

  // only the root gathers the arrays from everyone
  int* rbuf_final;

  if (rank == 0) {

	  // allocate a new array for the whole thing, same size as A
	  rbuf_final = (int*) malloc(N * sizeof(int));
  }
  MPI_Gather(mysums, length_each, MPI_INT, rbuf_final, length_each, MPI_INT, 0, comm);

  if (rank == 0) {
  
  	// print what you received
	printf("Rank 0, final result: \n");
	for (int v=0; v<N; v++) {
		printf(" %d \n", rbuf_final[v]);
	}
	
  }




  /* timing */
  MPI_Barrier(MPI_COMM_WORLD);
  double elapsed = MPI_Wtime() - tt;
  if (0 == rank) {
    printf("Time elapsed is %f seconds.\n", elapsed);
  }

  free(mysums);
  // do some error checking late

  MPI_Finalize();
  return 0;
}
