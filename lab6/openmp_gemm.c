#include <stdio.h>
#include <stdlib.h>
#include <cblas.h>
#include <assert.h> 
#include <pthread.h> 
#include <unistd.h>
#include<omp.h>
#include "defs.h"


#define CPU_CORES 2

typedef struct {
  int m;
  int n;
  int k;
  double *a;
  int lda;
  double *b;
  int ldb;
  double *c;
  int ldc;
  int start_row;
  int end_row;
} myarg_t;


void *part(void *args)
{
  myarg_t *arg = (myarg_t *) args;
  int i, j, p;
  for(i=arg -> start_row;i<arg -> end_row;i++)
  {
    for(j=0;j<arg -> n;j++)
    {
      for(p=0; p< arg -> k; p++)
      {
        arg -> c[i*arg -> n+j] = arg -> c[i*arg->n+j] + arg -> a[i*arg -> k+p] * arg -> b[p*arg -> n+j];
      }
    }
  }
  return NULL;
}

void MY_MMult(int m, int n, int k,
              double *a, int lda,
              double *b, int ldb,
              double *c, int ldc)
{
  pthread_t p[CPU_CORES];

  int i, rc;
  #pragma omp parallel for
  for(i=0;i<CPU_CORES;i++)
  {
    myarg_t args = {m, n, k, 
                    a, lda, 
                    b, ldb, 
                    c, ldc, 
                    i*m/CPU_CORES, 
                    (i+1)*m/CPU_CORES };
    part(&args);
  }
}
