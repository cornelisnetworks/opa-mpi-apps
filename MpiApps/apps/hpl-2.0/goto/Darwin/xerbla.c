#include <stdio.h>
#include <stdlib.h>

int xerbla_(char *message, int *info, long length){
  fprintf(stderr, " ** On entry to  %6s, parameter number %2d had an illegal value\n", message, *info); 
 exit(1);
}
