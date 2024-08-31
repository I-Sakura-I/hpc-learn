#include "stdio.h"
#include "stdlib.h"
#include "sys/time.h"
#include "time.h"
#include <cblas.h>

// 编译 gcc -o time_dgemm time_dgemm.c –lopenblas
// 运行 ./time_dgemm 1024
int main(int argc, char *argv[])
{
  if (argc < 2)
  {
    printf("Input Error\n");
    return 1;
  }

  printf("test!\n");
  int i, m, n, k;
  m = n = k = atoi(argv[1]);

  int sizeofa = m * k;
  int sizeofb = k * n;
  int sizeofc = m * n;
  int lda = m;
  int ldb = k;
  int ldc = m;

  double alpha = 1.2;
  double beta = 0.001;
  double dur = 0;
  double gf = 0;

  struct timeval start, finish;
  double duration;

  double *A = (double *)malloc(sizeof(double) * sizeofa);
  double *B = (double *)malloc(sizeof(double) * sizeofb);
  double *C = (double *)malloc(sizeof(double) * sizeofc);
  srand((unsigned)time(NULL));

  for (i = 0; i < sizeofa; i++)
  {
    A[i] = i % 3 + 1; // (rand() % 100) / 100.0;
  }

  for (i = 0; i < sizeofb; i++)
  {
    B[i] = i % 3 + 1; //(rand()%100)/10.0;
  }

  for (i = 0; i < sizeofc; i++)
  {
    C[i] = 0.1;
  }

  for (int i=0;i<10;i++)
  {
  printf("m=%d,n=%d,k=%d,alpha=%lf,beta=%lf,sizeofc=%d\n", m, n, k, alpha, beta, sizeofc);
  gettimeofday(&start, NULL);
  cblas_dgemm(CblasColMajor, CblasNoTrans, CblasNoTrans, m, n, k, alpha, A, lda, B, ldb, beta, C, ldc);
  gettimeofday(&finish, NULL);

  // 转成成秒数
  duration = (double)(finish.tv_sec - start.tv_sec) + (double)(finish.tv_usec - start.tv_usec) / 1.0e6;
  double gflops = 4.0 * m * n * k;
  gflops = gflops / duration * 1.0e-9;
  dur += duration / 10;
  gf += gflops / 10;

  FILE *fp;
  fp = fopen("timeDGEMM.txt", "a"); // 追加写
  fprintf(fp, "%dx%dx%d\t%lf s\t%lf GFLOPS\n", m, n, k, duration, gflops);
  fclose(fp);
  }
  printf("duration=%lf, glops=%lf",dur,gf);

  free(A);
  free(B);
  free(C);
  return 0;
}
