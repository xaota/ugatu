#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>

double f(long long int index){ // расчёт значения члена ряда с номером index
 return sin(1 / pow(index, 2)) / pow(5 * index - 1, 2);
}

double sum(long long int start, long long int end){ // выполняет поиск частичных сумм начиная с индекса start и до end
 double s = 0;
 for(long int i = start; i < end; ++i){
  s += f(i);
 }
 return s;
}

int main(int argc, char *argv[]){
 long long int N; // размерность ряда
 double s; // частичная сумма ряда
 int comm_size, comm_rank; // количество процессоров и номер процессора
 double global_start_time, global_end_time, global_work_time; // время работы

 N = argc > 1 ? atoll(argv[1]) : 10000; // если параметр N нельзя получить из коммандной строки, используем значение по умолчанию

 MPI_Init(&argc, &argv); // распараллеливание начинается отсюда
 MPI_Comm_rank(MPI_COMM_WORLD, &comm_rank); // каждый процесс узнает о количистве процессов всего
 MPI_Comm_size(MPI_COMM_WORLD, &comm_size); // каждый процесс узнаёт о собственном номере

 if(comm_rank == 0) global_start_time = MPI_Wtime(); // главный процесс запоминает время начала расчёта

 int partials = N / comm_size;
 long long int m = N % comm_size;
 long long int start = comm_rank * partials + 1;
 long long int end = (comm_rank + 1) * partials + 1;
 if(m > 0){
  if(comm_rank < m){
   start += comm_rank;
   end += comm_rank + 1;
  }else{
   start += m;
   end += m;
  }
 }

 double start_time, end_time; // локальные времена для времени расчёта частичных сумм
 start_time = MPI_Wtime(); // запоминаем время начала расчёта
 s = sum(start, end); // расчёт частичной суммы
 end_time = MPI_Wtime(); // запоминаем время конца расчёта
 double work_time = end_time - start_time; // получаем время конца расчёта
 MPI_Barrier(MPI_COMM_WORLD); // ждём все процессы

 if(comm_rank != 0){ // если процесс не главный
  MPI_Send(&s, 1, MPI_DOUBLE, 0, comm_size * comm_rank, MPI_COMM_WORLD); // посылаем главному процессу результат
 }

 if(comm_rank == 0){ // если процесс главный
  double partial_sum;
  double global_sum = s; // глобальная сумма пока это частичная сумма, посчитанная главным процессом
  MPI_Status status;
  for(int i = 1; i < comm_size; ++i){ // принимаем все частичные суммы от других процессов
   MPI_Recv(&partial_sum, 1, MPI_DOUBLE, i, comm_size * i, MPI_COMM_WORLD, &status);
   global_sum += partial_sum; // и доавляем к глобальной сумме
  }
  global_end_time = MPI_Wtime(); // глобальное время конца расчёта
  global_work_time = global_end_time - global_start_time;
 }

 fprintf(stderr, "proc %d of %d (%lf sec):\tsum from %lld to %lld:\t%1.8lf\n", comm_rank, comm_size, work_time, start, end-1, s); // вывод своего результата каждым процессом

 MPI_Barrier(MPI_COMM_WORLD); // ждём все процессы
 if(comm_rank == 0){
  fprintf(stderr, "global work time: %lf s; global sum of %lld elements = %lf\n", global_work_time, N, s); // вывод результата и времени работы всей программы
 }
 MPI_Finalize(); // конец параллелизации
 return 0; // конец программы
}
