#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define M 1024
#define N 1024
#define K 1024

/* Routine for computing C = A * B + C */
void MY_MMult(int m, int n, int k, double *a, double *b, double *c)
{
  int i, j, p;
  for (i = 0; i < m; i++) /* Loop over the rows of C */
  {
    for (j = 0; j < n; j++) /* Loop over the columns of C */
    {
      for (p = 0; p < k; p++)
      { /* Update C( i,j ) with the inner product of the ith row of A and the jth column of B */
        c[i*n+j] += a[i*k+p] * b[p*n+j];
      }
    }
  }
}

//Initialize an a*b matrix with floating point numbers between 0 and 1
void initialize(int a, int b, double *matrix)
{
  for (int i = 0; i < a; i++)
  {
    for(int j = 0; j < b; j++)
    {
      matrix[i*b+j]=(double)rand()/100;
    }
  }
}

int main()
{
  int m=M,n=N,k=K;
  
  double *a = (double *)malloc(m * k * sizeof(double));
  double *b = (double *)malloc(k * n * sizeof(double));
  double *c = (double *)malloc(m * n * sizeof(double));

  initialize(m,k,a);
  initialize(k,n,b);
  initialize(m,n,c);

  int rc=fork();

  MY_MMult(m,n,k,a,b,c);

  return 0;
}