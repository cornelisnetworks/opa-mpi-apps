#define BENCHMARK "OSU MPI One Sided MPI_Accumulate Latency Test"
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
    int         rank, destrank, nprocs, i;
    MPI_Group   comm_group, group;
    MPI_Win     win;
    MPI_Info    win_info;
    int         size, no_hints = 0;
    double      t_start=0.0, t_end=0.0;
    int         count, page_size;
    char        *A, *B;
    int         *s_buf, *r_buf;
    int         skip = 100;
    int         loop = 1000;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_group(MPI_COMM_WORLD, &comm_group);

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

    page_size = getpagesize();
    assert(page_size <= MAX_ALIGNMENT);

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

    s_buf =
        (int *) (((unsigned long) A + (page_size - 1)) /
                  page_size * page_size);
    r_buf =
        (int *) (((unsigned long) B + (page_size - 1)) /
                  page_size * page_size);

    for (i = 0; i < MAX_SIZE / sizeof(int); i++) {
        r_buf[i] = i;
        s_buf[i] = 2 * i;
    }

    if (rank == 0) {
        fprintf(stdout, HEADER);
        fprintf(stdout, "%-*s%*s\n", 10, "# Size", FIELD_WIDTH, "Latency (us)");
        fflush(stdout);
    }

    for (count = 0; count <= MAX_SIZE / sizeof(int); count = (count ? count << 1 : 1)) {
        size = count * sizeof(int);

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

                MPI_Accumulate(s_buf, count, MPI_INT, 1, 0, count, MPI_INT,
                        MPI_SUM, win);
                MPI_Win_complete(win);
                MPI_Win_post(group, 0, win);
                MPI_Win_wait(win);
            }

            t_end = MPI_Wtime ();
        } else {
            /*rank 1*/
            destrank = 0;

            MPI_Group_incl(comm_group, 1, &destrank, &group);
            MPI_Barrier(MPI_COMM_WORLD);

            for (i = 0; i < skip + loop; i++) {
                MPI_Win_post(group, 0, win);
                MPI_Win_wait(win);
                MPI_Win_start(group, 0, win);
                MPI_Accumulate(s_buf, count, MPI_INT, 0, 0, count, MPI_INT,
                        MPI_SUM, win);
                MPI_Win_complete (win);
            }
        }

        MPI_Barrier(MPI_COMM_WORLD);

        if (rank == 0) {
            fprintf(stdout, "%-*d%*.*f\n", 10, size, FIELD_WIDTH,
                    FLOAT_PRECISION, (t_end - t_start) * 1e6 / loop / 2);
            fflush(stdout);
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
    MPI_Finalize ();

    return 0;
}

/* vi: set sw=4 sts=4 tw=80: */
