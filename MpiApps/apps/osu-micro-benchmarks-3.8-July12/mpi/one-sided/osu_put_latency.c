#define BENCHMARK "OSU One Sided MPI_Put latency Test"
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
#include <string.h>
#include <assert.h>
#include <getopt.h>

#define MAX_ALIGNMENT 65536
#define MAX_SIZE (1<<22)
#define MYBUFSIZE (MAX_SIZE + MAX_ALIGNMENT)

#define SKIP_LARGE  10
#define LOOP_LARGE  100
#define LARGE_MESSAGE_SIZE  8192

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
    int         rank, destrank, nprocs, i;
    int         page_size;

    char       *A, *B;
    char       *s_buf, *r_buf;
    MPI_Group   comm_group, group;
    MPI_Win     win;
    MPI_Info    win_info;
    int         size, no_hints = 0;
    double      t_start=0.0, t_end=0.0;
    int         skip = 1000;
    int         loop = 10000;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if(nprocs != 2) {
        if(rank == 0) {
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

    MPI_Alloc_mem (MYBUFSIZE, MPI_INFO_NULL, &A);
    if (NULL == A) {
         fprintf(stderr, "[%d] Buffer Allocation Failed \n", rank);
         exit(-1);
    }

    if (no_hints == 0) {
        /* Providing MVAPICH2 specific hint to allocate memory 
         * in shared space. MVAPICH2 optimizes communication 
         * on windows created in this memory */
        MPI_Info_create(&win_info);
        MPI_Info_set(win_info, "alloc_shm", "true");

        MPI_Alloc_mem (MYBUFSIZE, win_info, &B);
    } else {
        MPI_Alloc_mem (MYBUFSIZE, MPI_INFO_NULL, &B);
    }
    if (NULL == B) {
         fprintf(stderr, "[%d] Buffer Allocation Failed \n", rank);
         exit(-1);
    }

    page_size = getpagesize();
    assert(page_size <= MAX_ALIGNMENT);

    s_buf =
        (char *) (((unsigned long) A + (page_size - 1)) /
                  page_size * page_size);
    r_buf =
        (char *) (((unsigned long) B + (page_size - 1)) /
                  page_size * page_size);

    memset(s_buf, 0, MAX_SIZE);
    memset(r_buf, 1, MAX_SIZE);

    if(rank == 0) {
        fprintf(stdout, HEADER);
        fprintf(stdout, "%-*s%*s\n", 10, "# Size", FIELD_WIDTH, "Latency (us)");
        fflush(stdout);
    }

    MPI_Comm_group(MPI_COMM_WORLD, &comm_group);

    for (size = 0; size <= MAX_SIZE; size = (size ? size * 2 : 1)) {
        if (size > LARGE_MESSAGE_SIZE) {
            loop = LOOP_LARGE;
            skip = SKIP_LARGE;
        }

        MPI_Win_create(r_buf, size, 1, MPI_INFO_NULL, MPI_COMM_WORLD, &win);

        if (rank == 0) {
            destrank = 1;

            MPI_Group_incl(comm_group, 1, &destrank, &group);
            MPI_Barrier(MPI_COMM_WORLD);

            for (i = 0; i < skip + loop; i++) {
                MPI_Win_start (group, 0, win);

                if (i == skip) {
                    t_start = MPI_Wtime ();
                }

                MPI_Put(s_buf, size, MPI_CHAR, 1, 0, size, MPI_CHAR, win);
                MPI_Win_complete(win);
                MPI_Win_post(group, 0, win);
                MPI_Win_wait(win);
            }

            t_end = MPI_Wtime ();
        } else {                
            /* rank=1 */
            destrank = 0;

            MPI_Group_incl(comm_group, 1, &destrank, &group);
            MPI_Barrier(MPI_COMM_WORLD);

            for (i = 0; i < skip + loop; i++) {
                MPI_Win_post(group, 0, win);
                MPI_Win_wait(win);
                MPI_Win_start(group, 0, win);
                MPI_Put(s_buf, size, MPI_CHAR, 0, 0, size, MPI_CHAR, win);
                MPI_Win_complete(win);
            }
        }

        MPI_Barrier(MPI_COMM_WORLD);

        if (rank == 0) {
            fprintf(stdout, "%-*d%*.*f\n", 10, size, FIELD_WIDTH,
                    FLOAT_PRECISION, (t_end - t_start) * 1.0e6 / loop / 2);
        }

        MPI_Group_free(&group);
        MPI_Win_free(&win);
    }

    if (no_hints == 0) {
        MPI_Info_free(&win_info);
    }
 
    MPI_Free_mem(A);
    MPI_Free_mem(B);

    MPI_Group_free(&comm_group);
    MPI_Finalize();

    return EXIT_SUCCESS;
}

/* vi: set sw=4 sts=4 tw=80: */
