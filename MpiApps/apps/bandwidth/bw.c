#include "mpi.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>


#define MYBUFSIZE (4*1024*1028)
#define MAX_REQ_NUM 100000

char s_buf1[MYBUFSIZE];
char r_buf1[MYBUFSIZE];


MPI_Request request[MAX_REQ_NUM];
MPI_Status my_stat[MAX_REQ_NUM];

int main(int argc,char *argv[])
{
    int  myid, numprocs, i;
    int size, loop, page_size;
    char *s_buf, *r_buf;
    double t_start=0.0, t_end=0.0, t=0.0;


    MPI_Init(&argc,&argv);
    MPI_Comm_size(MPI_COMM_WORLD,&numprocs);
    MPI_Comm_rank(MPI_COMM_WORLD,&myid);

    if ( argc < 3 ) {
       fprintf(stderr, "Usage: bw loop msg_size\n");
       MPI_Finalize();
       return 0;
    }    
    size=atoi(argv[2]);
    loop = atoi(argv[1]);

    if(size > MYBUFSIZE){
	     fprintf(stderr, "Maximum message size is %d\n",MYBUFSIZE);
	     MPI_Finalize();
	     return 0;
    }
	    
    if(loop > MAX_REQ_NUM){
	     fprintf(stderr, "Maximum number of iterations is %d\n",MAX_REQ_NUM);
	     MPI_Finalize();
	     return 0;
    }

    page_size = getpagesize();

    s_buf = (char*)(((unsigned long)s_buf1 + (page_size -1))/page_size * page_size);
    r_buf = (char*)(((unsigned long)r_buf1 + (page_size -1))/page_size * page_size);

    assert( (s_buf != NULL) && (r_buf != NULL) );

    for ( i=0; i<size; i++ ){
           s_buf[i]='a';
           r_buf[i]='b';
    }

    /*warmup */
    if (myid == 0)
    {
        for ( i=0; i< loop; i++ ) {
            MPI_Isend(s_buf, size, MPI_CHAR, 1, 100, MPI_COMM_WORLD, request+i);
        }

        MPI_Waitall(loop, request, my_stat);
        MPI_Recv(r_buf, 4, MPI_CHAR, 1, 101, MPI_COMM_WORLD, &my_stat[0]);

    }else{
        for ( i=0; i< loop; i++ ) {
	    MPI_Irecv(r_buf, size, MPI_CHAR, 0, 100, MPI_COMM_WORLD, request+i);
        }
	MPI_Waitall(loop, request, my_stat);
        MPI_Send(s_buf, 4, MPI_CHAR, 0, 101, MPI_COMM_WORLD);
    }

    MPI_Barrier(MPI_COMM_WORLD);

    if (myid == 0)
    {
        t_start=MPI_Wtime();
        for ( i=0; i< loop; i++ ) {
            MPI_Isend(s_buf, size, MPI_CHAR, 1, 100, MPI_COMM_WORLD, request+i);
        }

        MPI_Waitall(loop, request, my_stat);
        MPI_Recv(r_buf, 4, MPI_CHAR, 1, 101, MPI_COMM_WORLD, &my_stat[0]);

        t_end=MPI_Wtime();
        t = t_end - t_start;

    }else{
        for ( i=0; i< loop; i++ ) {
	    MPI_Irecv(r_buf, size, MPI_CHAR, 0, 100, MPI_COMM_WORLD, request+i);
        }
	MPI_Waitall(loop, request, my_stat);
        MPI_Send(s_buf, 4, MPI_CHAR, 0, 101, MPI_COMM_WORLD);
    }

    if ( myid == 0 ) {
       double tmp;
       tmp = ((size*1.0)/1.0e6)*loop; 
       fprintf(stdout,"%d\t%f\n", size, tmp/t);
    }

    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Finalize();
    return 0;
}

