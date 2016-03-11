/*
 * Copyright (C) 2002-2012 the Network-Based Computing Laboratory
 * (NBCL), The Ohio State University. 
 *
 * Contact: Dr. D. K. Panda (panda@cse.ohio-state.edu)
 *
 * For detailed copyright and licensing information, please refer to the
 * copyright file COPYRIGHT in the top level OMB directory.
 */
#ifndef OSU_COLL_H
#define OSU_COLL_H 1

#include <mpi.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

#define SKIP 200
#define SKIP_LARGE 10
#define LARGE_MESSAGE_SIZE 8192
#define MAX_ALIGNMENT 16384

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

int iterations = 1000;
int iterations_large = 100;
int print_size = 0;

void print_usage(int rank, const char * prog, int has_size)
{
    if (rank == 0) {
        if (has_size) {
            fprintf(stdout, " USAGE : %s [-m SIZE] [-i ITER] [-f] [-hv] \n", prog);
            fprintf(stdout, "  -m : Set maximum message size to SIZE.\n");
            fprintf(stdout, "       By default, the value of SIZE is 1MB.\n");
            fprintf(stdout, "  -i : Set number of iterations per message size to ITER.\n");
            fprintf(stdout, "       By default, the value of ITER is 1000 for small messages\n");
            fprintf(stdout, "       and 100 for large messages.\n");
        }

        else {
            fprintf(stdout, " USAGE : %s [-i ITER] [-f] [-hv] \n", prog);
            fprintf(stdout, "  -i : Set number of iterations to ITER.\n");
            fprintf(stdout, "       By default, the value of ITER is 1000.\n");
        }

        fprintf(stdout, "  -f : Print full format listing.  With this option\n");
        fprintf(stdout, "       the MIN/MAX latency and number of ITERATIONS are\n");
        fprintf(stdout, "       printed out in addition to the AVERAGE latency.\n");

        fprintf(stdout, "  -h : Print this help.\n");
        fprintf(stdout, "  -v : Print version info.\n");
        fprintf(stdout, "\n");
        fflush(stdout);
    }
}

void print_version()
{
        fprintf(stdout, HEADER);
        fflush(stdout);
}

int process_args (int argc, char *argv[], int rank, int * size, int * full)
{
    char c;

    if (size) {
        print_size = 1;
    }

    while ((c = getopt(argc, argv, ":hvfm:i:")) != -1) {
        switch (c) {
            case 'h':
                print_usage(rank, argv[0], size != NULL);
                return 1;

            case 'v':
                if (rank == 0) {
                    print_version();
                }

                return 1;

            case 'm':
                if (size) {
                    *size = atoi(optarg);
                    if (*size < 0) {
                        print_usage(rank, argv[0], size != NULL);
                        return -1;
                    }
                }

                else {
                    print_usage(rank, argv[0], 0);
                    return -1;
                }
                break;

            case 'i':
                iterations_large = atoi(optarg);
                iterations = iterations_large;
                if (iterations < 1) {
                    print_usage(rank, argv[0], size != NULL);
                    return -1;
                }
                break;

            case 'f':
                *full = 1;
                break;

            default:
                if (rank == 0) {
                    print_usage(rank, argv[0], size != NULL);
                }

                return -1;
        }
    }

    return 0;
}

void print_header (int rank, int full)
{
    if(rank == 0) {
        fprintf(stdout, HEADER);

        if (print_size) {
            fprintf(stdout, "%-*s", 10, "# Size");
            fprintf(stdout, "%*s", FIELD_WIDTH, "Avg Latency(us)");
        }

        else {
            fprintf(stdout, "# Avg Latency(us)");
        }

        if (full) {
            fprintf(stdout, "%*s", FIELD_WIDTH, "Min Latency(us)");
            fprintf(stdout, "%*s", FIELD_WIDTH, "Max Latency(us)");
            fprintf(stdout, "%*s\n", 12, "Iterations");
        }

        else {
            fprintf(stdout, "\n");
        }

        fflush(stdout);
    }
}

void print_data (int rank, int full, int size, double avg_time, double
        min_time, double max_time, int iterations)
{
    if(rank == 0) {
        if (print_size) {
            fprintf(stdout, "%-*d", 10, size);
            fprintf(stdout, "%*.*f", FIELD_WIDTH, FLOAT_PRECISION, avg_time);
        }

        else {
            fprintf(stdout, "%*.*f", 17, FLOAT_PRECISION, avg_time);
        }

        if (full) {
            fprintf(stdout, "%*.*f%*.*f%*d\n", 
                    FIELD_WIDTH, FLOAT_PRECISION, min_time,
                    FIELD_WIDTH, FLOAT_PRECISION, max_time,
                    12, iterations);
        }

        else {
            fprintf(stdout, "\n");
        }

        fflush(stdout);
    }
}

#endif
