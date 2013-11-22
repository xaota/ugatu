#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

double f(long long int n){
 return sin(1/(n*n))/((5 * n - 1)*(5*n - 1));
}

double sum(long long int N){
 double SUM = 0;
 #pragma loop count min(16)
 for(long long int n = 0; n < N; ++n){
  SUM += f(n+1);
 }
 return SUM;
}

int main(int argc, char *argv[]){
 long long int N = argc > 1 ? atol(argv[1]) : 1000000;

 printf("sum %lld = ", N);
 clock_t time = clock();
 double SUM;
 SUM = sum(N);
 printf("%lf\n", SUM);
 time = clock() - time;
 printf("evaluation time: %lf\n", (double)time / CLOCKS_PER_SEC);
 return 0;
}
