#define BENCHMARK "OSU Reduce_scatter Latency Test"
/*
 * Copyright (C) 2002-2012 the Network-Based Computing Laboratory
 * (NBCL), The Ohio State University.
 *
 * Contact: Dr. D. K. Panda (panda@cse.ohio-state.edu)
 */

/*
This program is available under BSD licensing.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are
met:

(1) Redistributions of source code must retain the above copyright
notice, this list of conditions and the following disclaimer.

(2) Redistributions in binary form must reproduce the above copyright
notice, this list of conditions and the following disclaimer in the
documentation and/or other materials provided with the distribution.

(3) Neither the name of The Ohio State University nor the names of
their contributors may be used to endorse or promote products derived
from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/

#include "osu_coll.h"

int main(int argc, char *argv[])
{
    int i, numprocs, rank, size, align_size;
    int skip;
    double latency = 0.0, t_start = 0.0, t_stop = 0.0;
    double timer=0.0;
    double avg_time = 0.0, max_time = 0.0, min_time = 0.0;
    float *sendbuf, *recvbuf, *s_buf1, *r_buf1;
    int *recvcounts;
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

    s_buf1 = r_buf1=NULL;
    s_buf1 = (float *) malloc(sizeof(float)*(max_msg_size/sizeof(float)) + MAX_ALIGNMENT);
    if(NULL == s_buf1) {
        fprintf(stderr, "s_buf1 malloc failed.\n");
        exit(1);
    }

    r_buf1 = (float *) malloc(sizeof(float)*((max_msg_size/numprocs+1)/sizeof(float)) + MAX_ALIGNMENT);
    if(NULL == s_buf1) {
        fprintf(stderr, "s_buf1 malloc failed.\n");
        exit(1);
    }
    
    align_size = getpagesize();
    sendbuf = (float *)(((unsigned long) s_buf1 + (align_size - 1)) / align_size
                    * align_size);
    memset(sendbuf, 1, max_msg_size);

    recvbuf = (float *)(((unsigned long) r_buf1 + (align_size - 1)) / align_size
                    * align_size);
       
     

    recvcounts = (int *)malloc( numprocs * sizeof(int) );

    for(size=1; size*sizeof(float)<= max_msg_size; size *= 2) {

        if(size > LARGE_MESSAGE_SIZE) {
            skip = SKIP_LARGE;
            iterations = iterations_large;
        } else {
            skip = SKIP;
            
        }

        int portion=0, remainder=0;
        portion=size/numprocs;
        remainder=size%numprocs;

        for (i=0; i<numprocs; i++){
            recvcounts[i]=0;
            if(size<numprocs){ 
                if(i<size)
                    recvcounts[i]=1;
            }
            else{
                if((remainder!=0) && (i<remainder)){
                    recvcounts[i]+=1;
                }
                recvcounts[i]+=portion;
            }
        }
        MPI_Barrier(MPI_COMM_WORLD);
        
        timer=0.0;
        for(i=0; i < iterations + skip ; i++) {
            t_start = MPI_Wtime();

            MPI_Reduce_scatter( sendbuf, recvbuf, recvcounts, MPI_FLOAT, MPI_SUM, MPI_COMM_WORLD );
            t_stop=MPI_Wtime();
            if(i>=skip){

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

        print_data(rank, full, sizeof(float)*size, avg_time, min_time, max_time, iterations);
        MPI_Barrier(MPI_COMM_WORLD);
    }

    free(s_buf1);
    free(r_buf1);

    MPI_Finalize();

    return EXIT_SUCCESS;
}
