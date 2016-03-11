#define BENCHMARK "OSU MPI All-to-Allv Personalized Exchange Latency Test"
/*
 * Copyright (C) 2002-2012 the Network-Based Computing Laboratory
 * (NBCL), The Ohio State University. 
 *
 * Contact: Dr. D. K. Panda (panda@cse.ohio-state.edu)
 *
 * For detailed copyright and licensing information, please refer to the
 * copyright file COPYRIGHT in the top level OMB directory.
 */

#include "osu_coll.h"

int main(int argc, char *argv[])
{
    int i = 0, rank = 0, size, numprocs, disp;
    int  skip, align_size;
    double latency=0.0, t_start = 0.0, t_stop = 0.0;
    double timer=0.0;
    double avg_time = 0.0, max_time = 0.0, min_time = 0.0;
    char *sendbuf=NULL, *recvbuf=NULL, *s_buf1=NULL, *r_buf1=NULL;
    int *rdispls=NULL, *recvcounts=NULL, *sdispls=NULL, *sendcounts=NULL;
    int max_msg_size = 1048576, full = 0;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &numprocs);

    if (process_args(argc, argv, rank, &max_msg_size, &full)) {
        MPI_Finalize();
        return EXIT_SUCCESS;
    }

    if(numprocs < 2) {
        if(rank == 0) {
            fprintf(stderr, "This test requires at least two processes\n");
        }

        MPI_Finalize();

        return EXIT_FAILURE;
    }

    print_header(rank, full);


    recvcounts = (int *) malloc (numprocs*sizeof(int));
    if(NULL == recvcounts) {
        fprintf(stderr, "malloc failed.\n");
        exit(1);
    }

    sendcounts = (int *) malloc (numprocs*sizeof(int));
    if(NULL == recvcounts) {
        fprintf(stderr, "malloc failed.\n");
        exit(1);
    }

    rdispls = (int *) malloc (numprocs*sizeof(int));
    if(NULL == rdispls) {
        fprintf(stderr, "malloc failed.\n");
        exit(1);
    }

    sdispls = (int *) malloc (numprocs*sizeof(int));
    if(NULL == rdispls) {
        fprintf(stderr, "malloc failed.\n");
        exit(1);
    }

    s_buf1 = (char *)malloc(sizeof(char) * max_msg_size * numprocs +
            MAX_ALIGNMENT);

    if(NULL == s_buf1) {
        fprintf(stderr, "malloc failed.\n");

        exit(1);
    }

    r_buf1 = (char *) malloc (sizeof(char) * max_msg_size * numprocs +
            MAX_ALIGNMENT);

    if(NULL == r_buf1) {
        fprintf(stderr, "malloc failed.\n");

        exit(1);
    }


    align_size = getpagesize();
    sendbuf = (char *)(((unsigned long) s_buf1 + (align_size - 1)) / align_size
            * align_size);
    recvbuf = (char *)(((unsigned long) r_buf1 + (align_size - 1)) / align_size
            * align_size);


    MPI_Barrier(MPI_COMM_WORLD);

    for(size = 1; size <= max_msg_size; size *= 2) {
        if(size > LARGE_MESSAGE_SIZE) {
            skip = SKIP_LARGE;
            iterations = iterations_large;
        }

        else {
            skip = SKIP;
            
        }

        disp =0;
        for ( i = 0; i < numprocs; i++) {
            recvcounts[i] = size;
            sendcounts[i] = size;
            rdispls[i] = disp;
            sdispls[i] = disp;
            disp += size;

        }

        MPI_Barrier(MPI_COMM_WORLD);

        timer=0.0;
        for(i = 0; i < iterations + skip; i++) {
            t_start = MPI_Wtime();

              MPI_Alltoallv(sendbuf, sendcounts, sdispls, MPI_CHAR, recvbuf, recvcounts, rdispls, MPI_CHAR,
                      MPI_COMM_WORLD);

            t_stop = MPI_Wtime();

            if(i>=skip)
            {
                timer+=t_stop-t_start;
            }
            MPI_Barrier(MPI_COMM_WORLD);  
        }

        latency = (double)(timer * 1e6) / iterations;

        MPI_Reduce(&latency, &min_time, 1, MPI_DOUBLE, MPI_MIN, 0,
                MPI_COMM_WORLD);
        MPI_Reduce(&latency, &max_time, 1, MPI_DOUBLE, MPI_MAX, 0,
                MPI_COMM_WORLD);
        MPI_Reduce(&latency, &avg_time, 1, MPI_DOUBLE, MPI_SUM, 0,
                MPI_COMM_WORLD);
        avg_time = avg_time/numprocs;
        print_data(rank, full, size, avg_time, min_time, max_time, iterations);

        MPI_Barrier(MPI_COMM_WORLD);
    }

    free(s_buf1);
    free(r_buf1);

    MPI_Finalize();

    return EXIT_SUCCESS;
}

/* vi: set sw=4 sts=4 tw=80: */
