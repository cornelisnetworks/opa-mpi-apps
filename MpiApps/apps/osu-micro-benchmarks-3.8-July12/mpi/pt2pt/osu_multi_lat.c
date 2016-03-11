#define BENCHMARK "OSU MPI Multi Latency Test"
/*
 * Copyright (C) 2002-2012 the Network-Based Computing Laboratory
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

#define MAX_ALIGNMENT (16384)
#define MAX_MSG_SIZE (1<<7)
#define MAX_STEPS    (22+1)
#define MAXBUFSIZE (MAX_MSG_SIZE + MAX_ALIGNMENT)
#define LARGE_MSG_SIZE (8192)
#define LOOP_SMALL  10000
#define SKIP_SMALL  100
#define LOOP_LARGE  1000
#define SKIP_LARGE  10

char s_buf1[MAXBUFSIZE];
char r_buf1[MAXBUFSIZE];
char *s_buf, *r_buf;


static void multi_latency(int rank, int pairs);

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

int main(int argc, char* argv[])
{
    int align_size, rank, nprocs; 
    int pairs;

    MPI_Init(&argc, &argv);

    align_size = getpagesize();
    s_buf =
        (char *) (((unsigned long) s_buf1 + (align_size - 1)) /
                  align_size * align_size);
    r_buf =
        (char *) (((unsigned long) r_buf1 + (align_size - 1)) /
                  align_size * align_size);

    memset(s_buf, 0, MAX_MSG_SIZE);
    memset(r_buf, 0, MAX_MSG_SIZE);

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);

    pairs = nprocs/2;

    if(rank == 0) {
        fprintf(stdout, HEADER);
        fprintf(stdout, "%-*s%*s\n", 10, "# Size", FIELD_WIDTH, "Latency (us)");
/*hjw: */
        fprintf(stdout, "# [ pairs: %d ] \n", pairs);

        fflush(stdout);
    }

    MPI_Barrier(MPI_COMM_WORLD);

    multi_latency(rank, pairs);
    
    MPI_Barrier(MPI_COMM_WORLD);

    MPI_Finalize();

    return EXIT_SUCCESS;
}

static void multi_latency(int rank, int pairs)
{
    int size, partner;
    int loop, i, skip;
    double t_start = 0.0, t_end = 0.0,
           latency = 0.0, total_lat = 0.0,
           avg_lat = 0.0;

    MPI_Status reqstat;


    for(size = 0; size <= MAX_MSG_SIZE; size  = (size ? size * 2 : 1)) {

        MPI_Barrier(MPI_COMM_WORLD);

        if(size > LARGE_MSG_SIZE) {
            loop = LOOP_LARGE;
            skip = SKIP_LARGE;
        } else {
            loop = LOOP_SMALL;
            skip = SKIP_SMALL;
        }

        if (rank < pairs) {
            partner = rank + pairs;

            for (i = 0; i < loop + skip; i++) {

                if (i == skip) {
                    t_start = MPI_Wtime();
                    MPI_Barrier(MPI_COMM_WORLD);
                }

                MPI_Send(s_buf, size, MPI_CHAR, partner, 1, MPI_COMM_WORLD);
                MPI_Recv(r_buf, size, MPI_CHAR, partner, 1, MPI_COMM_WORLD,
                         &reqstat);
            }

            t_end = MPI_Wtime();

        } else {
            partner = rank - pairs;

            for (i = 0; i < loop + skip; i++) {

                if (i == skip) {
                    t_start = MPI_Wtime();
                    MPI_Barrier(MPI_COMM_WORLD);
                }

                MPI_Recv(r_buf, size, MPI_CHAR, partner, 1, MPI_COMM_WORLD,
                         &reqstat);
                MPI_Send(s_buf, size, MPI_CHAR, partner, 1, MPI_COMM_WORLD);
            }

            t_end = MPI_Wtime();
        }

        latency = (t_end - t_start) * 1.0e6 / (2.0 * loop);

        MPI_Reduce(&latency, &total_lat, 1, MPI_DOUBLE, MPI_SUM, 0, 
                   MPI_COMM_WORLD);

        avg_lat = total_lat/(double) (pairs * 2);

        if(0 == rank) {
            fprintf(stdout, "%-*d%*.*f\n", 10, size, FIELD_WIDTH,
                    FLOAT_PRECISION, avg_lat);
            fflush(stdout);
        }
    }
}

/* vi: set sw=4 sts=4 tw=80: */
