#include <stdio.h>
#include <string.h>
#include<stdlib.h>
#include<mpi.h>

#define M 8
#define N 16
#define K 4

typedef struct {
  int m;
  int n;
  int k;
  double *a;
  double *b;
  int start_row;
  int end_row;
} myarg_t;

//初始化尺寸为(l,w)的矩阵
void initialize(int l, int w, double *matrix)
{
  for (int i = 0; i < l; i++)
  {
    for(int j = 0; j < w; j++)
    {
      matrix[i*w+j]=(double)(rand()%1000)/1000;
    }
  }
}

//初始化尺寸为(l,w)的零矩阵
void zeros(int l, int w, double *matrix)
{
  for (int i = 0; i < l; i++)
  {
    for(int j = 0; j < w; j++)
    {
      matrix[i*w+j]=0;
    }
  }
}

//打印尺寸为(l,w)的矩阵
void prtm(int l, int w, double *matrix)
{
  for(int i=0;i<l;i++)
  {
    for(int j=0;j<w;j++)
    {
      printf("%.3f  ",matrix[i*w+j]);
    }
    printf("\n");
  }
}

//对分块矩阵进行运算
double *part(void *args)
{
  myarg_t *arg = (myarg_t *) args;
  int i, j, p;
  double *c=malloc((arg -> end_row - arg -> start_row) * arg -> n * sizeof(double));
  for(i=arg -> start_row;i<arg -> end_row;i++)
  {
    for(j=0;j<arg -> n;j++)
    {
      double sum=0;
      for(p=0; p< arg -> k; p++)
      {
        sum += arg -> a[i*arg -> k+p] * arg -> b[p*arg -> n+j];
      }
      c[(i-arg -> start_row)*arg -> n+j]=sum;
    }
  }
  return c;
}

//多线程运算矩阵乘法
void MY_MMult(int m, int n, int k,
              double *a, double *b, double *c)
{
    int rank, size;
    MPI_Status status;

    MPI_Init(0,0);  // 初始化 MPI 环境
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);  // 获取当前进程的 rank
    MPI_Comm_size(MPI_COMM_WORLD, &size);  // 获取进程总数
    
    double *message=(double *)malloc(m / size * n * sizeof(double));
    

    if(rank!=0)
    {

      myarg_t args = {m, n, k, 
                  a, b,
                  rank*m/size, 
                  (rank+1)*m/size};
      message=part(&args);
      MPI_Send(message, m/size*n, 
        MPI_DOUBLE, 0, 99, MPI_COMM_WORLD);
    }
    else
    {
      myarg_t args = {m, n, k, 
                      a, b, 0, 
                      m/size};
      double *first = part(&args);
      for (int i = 0; i < m/size*n; i++)
      {
        c[i]=first[i];
      }
      for (int source = 1; source < size; source++)
      {
			MPI_Recv(c+source*m/size*n, m/size*n, MPI_DOUBLE, source,
                     99, MPI_COMM_WORLD, &status);
		  }

      printf("矩阵A的值为：\n");
      prtm(m,k,a);

      printf("矩阵B的值为：\n");
      prtm(k,n,b);

      printf("矩阵C的值为：\n");
      prtm(m,n,c);

      free(message);
      
      MPI_Finalize();
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
  zeros(m,n,c);

  MY_MMult(m,n,k,a,b,c);

  free(a);
  free(b);
  free(c);
}