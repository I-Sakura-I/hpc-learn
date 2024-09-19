#include <stdio.h>
#include <stdlib.h>
#include <cblas.h>
#include <assert.h> 
#include <pthread.h> 
#include <unistd.h>
#include "defs.h"


#define CPU_CORES 2
#define scale 4 //the length of side of each block

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

void block (int m, int n, int k,
            double *a, int lda,
            double *b, int ldb,
            double *c, int ldc,
            int start_row, int start_col)
{
    int i, j, p;
    for(i = start_row; i < start_row + scale; i++)
    {
        for(j = start_col; j < start_col + scale; j++)
        {
            for(p = 0; p < k; p++)
            {
                C(i, j) += A(i, p) * B(p, j);
            }
        }
    }
}

void *part(void *args)
{
  myarg_t *arg = (myarg_t *) args;
  int m = arg->m, start_row = arg->start_row,
          n = arg->n, end_row = arg->end_row,
          lda = arg->lda, ldb = arg->ldb, 
          ldc = arg->ldc, k = arg->k;
  double *a = arg->a, *b = arg->b, *c = arg->c;
  int i, j, p;
  for(i=start_row;i<end_row;i += scale)
  {
    for(j=0;j<n;j += scale) //disassemble each part into several blocks
    {
      block(m, n, k, 
            a, lda, 
            b, ldb, 
            c, ldc, 
            i, j);
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
  for(i=0;i<CPU_CORES;i++)
  {
    myarg_t args = {m, n, k, 
                    a, lda, 
                    b, ldb, 
                    c, ldc, 
                    i*m/CPU_CORES, 
                    (i+1)*m/CPU_CORES};
    rc = pthread_create(&p[i], NULL, part, &args); assert(rc == 0);
  }
  for(i=0;i<CPU_CORES;i++)
  {
    rc = pthread_join(p[i], NULL); assert(rc == 0); 
  }
}
