// mpicc -Wall  -fopenmp hybrid_calc_Pi.c
// mpirun -np 2 ./a.out

#include <mpi.h> /* MPI header file */
#include <omp.h> /* OpenMP header file */
#include <stdio.h>
#include <stdlib.h>
#define NUM_STEPS 100000000
#define MAX_THREADS 8

int main(int argc, char *argv[]) {
  int nprocs, myid;
  int tid, nthreads, nbin;
//   double start_time, end_time;
  double pi, Psum = 0.0, sum[MAX_THREADS] = {0.0};
  double step = 1.0 / (double)NUM_STEPS;

  /* initialize for MPI */
  MPI_Init(&argc, &argv); /* starts MPI */
  /* get number of processes */
  MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
  /* get this process's number (ranges from 0 to nprocs - 1) */
  MPI_Comm_rank(MPI_COMM_WORLD, &myid);
  nbin = NUM_STEPS / nprocs;

  #pragma omp parallel private(tid)
  {
    int i;
    double x;
    nthreads = omp_get_num_threads();
    tid = omp_get_thread_num();
    for (i = nbin * myid + tid; i < nbin * (myid + 1);
         i += nthreads) { /* changed*/
      x = (i + 0.5) * step;
      sum[tid] += 4.0 / (1.0 + x * x);
    }
  }

  for (tid = 0; tid < nthreads; tid++) /*sum by each mpi process*/
    Psum += sum[tid] * step;

	// printf("Hello from thread %2d out of %d from process %d\n",
	// tid, nprocs, myid);

  MPI_Reduce(&Psum, &pi, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD); /* added */

  if (myid == 0) {
    printf("parallel program results with %d processes:\n", nprocs);
    printf("pi = %g (%17.15f)\n", pi, pi);
  }
  MPI_Finalize();
  return 0;

}
