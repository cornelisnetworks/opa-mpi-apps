#define BENCHMARK "OSU MPI One Sided MPI_Accumulate Passive Latency Test"
/*
 * Copyright (C) 2003-2012 the Network-Based Computing Laboratory
 * (NBCL), The Ohio State University.
 *
 * Contact: Dr. D. K. Panda (panda@cse.ohio-state.edu)
 *
 * For detailed copyright and licensing information, please refer to the
 * copyright file COPYRIGHT in the top level OMB directory.
 */

#include <mpi.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <getopt.h>

#define SKIP_LARGE  10
#define LOOP_LARGE  100
#define LARGE_MESSAGE_SIZE  8192

#define MAX_ALIGNMENT 65536
#define MAX_SIZE (1<<22)
#define MYBUFSIZE (MAX_SIZE + MAX_ALIGNMENT)

#ifdef PACKAGE_VERSION
#   define HEADER "# " BENCHMARK " v" PACKAGE_VERSION "\n"
#else
#   define HEADER "# " BENCHMARK "\n"
#endif

#ifndef FIELD_WIDTH
#   define FIELD_WIDTH 20
#endif

#ifndef FLOAT_PRECISION
#   define FLOAT_PRECISION 2
#endif

int main (int argc, char *argv[])
{
    int         rank, nprocs, i;
    int         count, page_size;
    int         *s_buf=NULL, *r_buf=NULL;
    int         *A = NULL, *B = NULL;
    int         size, no_hints = 0;
    MPI_Win     win;
    MPI_Info    win_info;
    double      t_start = 0.0, t_end = 0.0;
    int skip = 100;
    int loop = 1000;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (nprocs != 2) {
        if (rank == 0) {
            fprintf(stderr, "This test requires exactly two processes\n");
        }

        MPI_Finalize();

        return EXIT_FAILURE;
    }

    while (1) {
        static struct option long_options[] =
            {{"no-hints", no_argument, NULL, 'n'},
             {0, 0, 0, 0}};
        int option, index;

        option = getopt_long (argc, argv, "n::",
                            long_options, &index);

        if (option == -1) {
            break;
        }

        switch (option) {
            case 'n':
                no_hints = 1;
                break;
            default:
                if (rank == 0) {
                    fprintf(stderr, "Invalid Option \n");
                }
                MPI_Finalize();
                return EXIT_FAILURE;
        }
    }

    MPI_Alloc_mem(MYBUFSIZE, MPI_INFO_NULL, &A);
    if (A == NULL) {
		fprintf(stderr, "Allocation failed \n");
		MPI_Finalize();
		return EXIT_FAILURE;
    }

    if (no_hints == 0) {
        /* Providing MVAPICH2 specific hint to allocate memory 
         * in shared space. MVAPICH2 optimizes communication          
         * on windows created in this memory */
        MPI_Info_create(&win_info);
        MPI_Info_set(win_info, "alloc_shm", "true");
    
        MPI_Alloc_mem(MYBUFSIZE, win_info, &B);
    } else { 
        MPI_Alloc_mem(MYBUFSIZE, MPI_INFO_NULL, &B);
    }
    if (B == NULL) {
		fprintf(stderr, "Allocation failed \n");
		MPI_Finalize();
		return EXIT_FAILURE;
    }
    page_size = getpagesize();
    assert(page_size <= MAX_ALIGNMENT);

    s_buf =
        (int *) (((unsigned long) A + (page_size - 1)) /
                  page_size * page_size);
    r_buf =
        (int *) (((unsigned long) B + (page_size - 1)) /
                  page_size * page_size);

    for (i = 0; i < MAX_SIZE / sizeof(int); i++) {
        s_buf[i] = 2 * i;
    }

    if (rank == 0) {
        fprintf(stdout, HEADER);
        fprintf(stdout, "%-*s%*s\n", 10, "# Size", FIELD_WIDTH, "Latency (us)");
        fflush(stdout);
    }

    for (count = 0; count <= MAX_SIZE/sizeof(int); count = (count ? count*2 : count+1)) {
        size = count * sizeof(int);

        if (size > LARGE_MESSAGE_SIZE) {
            loop = LOOP_LARGE;
            skip = SKIP_LARGE;
        }

        MPI_Win_create(r_buf, size, 1, MPI_INFO_NULL, MPI_COMM_WORLD, &win);

        if (rank == 0) {

            for (i = 0; i < skip + loop; i++) {

                if (i == skip) {
                    t_start = MPI_Wtime ();
                }
                MPI_Win_lock(MPI_LOCK_EXCLUSIVE, 1, 0, win); 
                MPI_Accumulate(s_buf, count, MPI_INT, 1, 0, count, MPI_INT,
                        MPI_SUM, win);
                MPI_Win_unlock(1, win);
            }

            t_end = MPI_Wtime ();
        }

        MPI_Barrier(MPI_COMM_WORLD);

        if (rank == 0) {
            fprintf(stdout, "%-*d%*.*f\n", 10, size, FIELD_WIDTH,
                    FLOAT_PRECISION, (t_end - t_start) * 1e6 / loop );
            fflush(stdout);
        }

        MPI_Win_free(&win);
    }

    if (no_hints == 0) {
        MPI_Info_free(&win_info);   
    }
 
    MPI_Free_mem(A);
	MPI_Free_mem(B);
    MPI_Finalize ();

    return 0;
}

/* vi: set sw=4 sts=4 tw=80: */
