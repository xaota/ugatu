#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>
#include <mpi.h>
#include <time.h>
#define SEED 666 // число для розыгрыша случайных чисел

static inline void generateArray(double *array, int length){ // Заполняет массив случайными числами
 for(int i = 0; i < length; ++i) array[i] = ((double)(rand() % 101)) / 100.0;
}
static inline int index(int i, int j, int l){ // возвращает номер [i,j] элемента матрицы в одномерном массиве, длинна каждой строки матрицы = l
 return(l * i + j);
}
void out(char *format, ... ){ // Вывод
 va_list ptr;
 va_start(ptr, format);
 vfprintf(stderr, format, ptr);
 va_end(ptr);
}
void printMatrix(double *matrix, char *name, int n, int l){ // Вывод матрицы на экран, n - количество строк, l - количество столбцов
 out("Матрица %s:\n", name); // Пишем название матрицы
 for(int i = 0; i < n; ++i){
  for(int j = 0; j < l; ++j){
   out("%.2lf ", matrix[index(i, j, l)]); // пишем [i,j] элемент матрицы
  }
  out("\n");
 }
}
void mult(double *a, double *b, double *c, int n, int l){ // умножение матриц (n - количество строк матриц A и С; l - количество столбцов матриц A, B и C)
 for(int i = 0; i < n; ++i){
  for(int j = 0; j < l; ++j){
   double sum = 0;
   for(int k = 0; k < l; ++k){
    sum += a[index(i,k,l)] * b[index(k,j,l)];
   }
   c[index(i,j,l)] = sum;
  }
 }
}
double norm(double *matrix, int n, int l){ // Евклидова норма матрицы (n - количество строк, l - количество столбцов)
 int length = n * l; // находим длинну одномерного массива
 double sqrSum = 0;
 for(int i = 0; i < length; ++i){
  sqrSum += matrix[i] * matrix[i]; // считаем сумму квадратов
 }
 return sqrt(sqrSum); // возвращаем корень из суммы квадратов
}

int main(int argc, char* argv[]){
 // если параметры нельзя получить из коммандной строки, используем значения по умолчанию
 int L = argc > 1 ? atoi(argv[1]) : 10; // количество столбцов матрицы A. В итоге A будет иметь разимерность [N = 10 * L, L]

 // Сначала равномерно распределяем матрицу между процессами, и смотрим, на каких узлах расчёт части завершился быстрее
 // Затем, в зависимости от времени расчёта на узле, процессам посылается большая или меньшая часть матрицы для неравномерного расчёта.

 int N = 10 * L; // Количество строк матрицы A
 int sizeA = N*L, sizeB = L*L, sizeC = sizeA; // Количество элементов в матрицах A, B и C

 int comm_size, comm_rank; // Переменные для хранения количества процессов и номера текущего процесса

 double *A, *B, *C; // Указатели на матрицы, причём матрицы A и C полностью будут храниться лишь на галавном процессе, а на остальных лишь их части
 double *matrix; // указатель для памяти под матрицы на главном процессе

 double global_start_time, global_end_time, global_work_time; // время работы
 double start_time, end_time, work_time; // локальные времена для сохранения времени расчёта каждого процесса
 double *times; // массив для хранения времен работы каждого процесса на главном процессе
 int *partialsN; // массив для храенения количества посданных процессам строк

 MPI_Init(&argc, &argv); // Инициализируем MPI
 MPI_Comm comm = MPI_COMM_WORLD;
 MPI_Comm_size(comm, &comm_size); // Указываем каждому процессу общеее количество процессов
 MPI_Comm_rank(comm, &comm_rank); // Указываем каждому процессу его номер

 if(comm_rank == 0){ // код, выполняемый на главном процессе инициализирует матрицы
  matrix = (double*)malloc((sizeA + sizeB + sizeC) * sizeof(double)); // Создадим общую область памяти под все три матрицы на главном процессе
  srand(SEED); // Для заполнения матриц случайными числами сначала разыграем счётчик
  generateArray(matrix, sizeA + sizeB); // Заполняем матрицы A и B случайными числами
  A = matrix; // Укажем указатели на разные области памяти в общем массиве, как разные матрицы. Это матрица A
  B = A + sizeA; // B - матрица, элементы которой начинаются после элементов матрицы A
  C = B + sizeB; // C - матрица, элементы которой начинаются после элементов матрицы B
  // Тестовый вывод матриц
  // printMatrix(A, "A", N, L);
  // printMatrix(B, "B", L, L);
 }

 if(comm_rank == 0){
  times = (double*)malloc(comm_size * sizeof(double));
  partialsN = (int*)malloc(comm_size * sizeof(int));
  out("Гомогенный случай; Процессов: %d\n", comm_size);
  global_start_time = MPI_Wtime(); // главный процесс запоминает время начала расчёта
 }

 // Каждый процесс рассчитает, какие именно строки матрицы A он будет умножать на матрицу B
 int partials = N / comm_size,
     m = N % comm_size,
     start = comm_rank * partials,
     end = (comm_rank + 1) * partials;

 if(m > 0){
  if(comm_rank < m){
   start += comm_rank;
   end += comm_rank + 1;
  }else{
   start += m;
   end += m;
  }
 }

 // MPI_Barrier(comm); // ждём все процессы (просто для красивого вывода)
 // out("proc %d: start %d, end %d\n", comm_rank, start, end);

 int partialN = end - start;
 start *= L;
 end *= L;
 int partialSize = end - start;

 if(comm_rank > 0){ // на каждом процессе, кроме главного выделяем память под кусок матрицы A, матрицу B и кусок матрицы C
  matrix = (double*)malloc((partialSize + sizeB + partialSize) * sizeof(double));
  A = matrix;
  B = A + partialSize;
  C = B + sizeB;
 }

 MPI_Barrier(comm); // ждём все процессы (чтобы точно у всех была выделена память)
 MPI_Status status;
 /* План обмена данными
  i-тый процесс посылает нулевому (главному) номер элемента start из массива, хранящего матрицу A, с которого нужно начать посылать элементы i-тому процесу // метка сообщения comm_rank
  i-тый процесс посылает нулевому количество partialN требуемых строк матрицы A (главный процесс узнает количество элементов, умножив количество строк на L) // метка сообщения comm_rank + comm_size
  нулевой процесс посылает i-тому кусок матрицы A, начинающийся с элемента start и длинной partialSize // метка сообщения comm_rank + 2 * comm_size
  *** расчёт ***
  i-тый процесс посылает главному результат - сначала посылается точка start, куда начинать записывать элементы в C // метка сообщения comm_rank + 3 * comm_size
  i-тый процесс посылает главному сами элементы матрицы C // метка сообщения comm_rank + 4 * comm_size
  i-тый процесс посылает главному своё время расчёта // метка сообщения comm_rank + 5 * comm_size
 */
 // теперь все процессы должны сообщить главному, какие именно они хотят элементы матрицы A
 if(comm_rank > 0){
  // MPI_Send(адрес данных, количество данных, номер процесса получателя, метка сообщения, среда обмена)
  MPI_Send(&start, 1, MPI_INT, 0, comm_rank, comm); // стартовый элемент A
  MPI_Send(&partialN, 1, MPI_INT, 0, comm_rank + comm_size, comm); // количество строк A
 }
 // главный процесс собирает эти данные и в ответ пошлёт необходимые элементы
 if(comm_rank == 0){ // Код, выполняющийся на главном процессе, разошлёт оставшимся процессам определённые строки матрицы A и матрицу B
  partialsN[0] = partialN;
  out("Процесс (0) получит строк матрицы A: %d\n", partialsN[0]);
  for(int i = 1; i < comm_size; ++i){ // по каждому дочернему процессу
   // MPI_Recv(адрес данных, количество данных, номер процесса отправителя, метка сообщения, среда обмена, статус сообщения)
   int partial_start, partial_size;
   // принимаем входящие запросы на стартовый элемент и их количество
   MPI_Recv(&partial_start, 1, MPI_INT, i, i, comm, &status);
   MPI_Recv(&partial_size, 1, MPI_INT, i, i + comm_size, comm, &status);
   // сохраняем количество требуемых строк и вычисляем количество требуемых элементов
   partialsN[i] = partial_size;
   out("Процесс (%d) получит строк матрицы A: %d\n", i, partialsN[i]);
   partial_size *= L;
   // отправляем в ответ кусок массива c матрицей A
   MPI_Send(A + partial_start, partial_size, MPI_DOUBLE, i, i + 2 * comm_size, comm);
  }
 }
 if(comm_rank > 0){ // дочерние процессы получают свой кусок матрицы A
  MPI_Recv(A, partialSize, MPI_DOUBLE, 0, comm_rank + 2 * comm_size, comm, &status);
 }
 MPI_Bcast(B, sizeB, MPI_DOUBLE, 0, comm); // Все процессы получают матрицу B такую же, как у процесса 0

 //MPI_Barrier(comm); // последняя точка синхронизации перед стартом расчёта. (Вообще говоря, НЕ нужна)
 /* // Вывод данных для уверенности успешности обмена
  char str1[80], str2[80];
  sprintf(str1, "%s %d %s","A (", comm_rank ,")");
  sprintf(str2, "%s %d %s","B (", comm_rank ,")");
  printMatrix(A, str1, partialN, L);
  printMatrix(B, str2, L, L);
 */

 // все процессы считают свою часть
 start_time = MPI_Wtime(); // запоминаем время начала расчёта
 mult(A, B, C, partialN, L); // частично умножаем матрицы
 end_time = MPI_Wtime(); // запоминаем время конца расчёта
 work_time = end_time - start_time; // получаем время конца расчёта

 MPI_Barrier(comm); // точка синхронизации после окончания расчёта

 /* // Вывод данных для уверенности успешности
  char str3[80];
  sprintf(str3, "%s %d %s","С (", comm_rank ,")");
  printMatrix(C, str3, partialN, L);
 */

 if(comm_rank > 0){ // Все дочерние процессы посылают свои части матрицы C главному процессу
  MPI_Send(&start, 1, MPI_INT, 0, comm_rank+comm_size * 3, comm); // стартовый элемент С
  MPI_Send(C, partialSize, MPI_DOUBLE, 0, comm_rank + comm_size * 4, comm); // элементы С
  MPI_Send(&work_time, 1, MPI_DOUBLE, 0, comm_rank + comm_size * 5, comm); // время работы процесса
 }
 // главный процесс собирает матрицу C и считает её Евклидову норму
 if(comm_rank == 0){
  out("Получено время расчёта процесса (0) : %lf с.\n", work_time);
  times[0] = work_time; // сохраняем время расчёта главного процесса
  for(int i = 1; i < comm_size; ++i){ // по каждому дочернему процессу
   int partial_start, partial_size = partialsN[i] * L;
   // принимаем входящие запросы на стартовый элемент и их количество
   MPI_Recv(&partial_start, 1, MPI_INT, i, i + comm_size * 3, comm, &status);
   // принимаем кусок матрицы C
   MPI_Recv(C + partial_start, partial_size, MPI_DOUBLE, i, i + comm_size * 4, comm, &status);
   // Принимаем время расчёта
   MPI_Recv(times + i, 1, MPI_DOUBLE, i, i + comm_size * 5, comm, &status);
   out("Получено время расчёта процесса (%d) : %lf с.\n", i, times[i]);
  }
 }
 MPI_Barrier(comm); // последняя точка синхронизации (Не нужна (?))
 if(comm_rank == 0){
  // printMatrix(C, "C", N, L);
  double normMatrix1 = norm(C, N, L); // считаем норму матрицы
  global_end_time = MPI_Wtime(); // глобальное время конца расчёта
  global_work_time = global_end_time - global_start_time;
  out("Количество строк матрицы A: %d\nНорма матрицы: %lf\nВремя расчёта: %lf с.\n", N, normMatrix1, global_work_time);
 }

 MPI_Barrier(comm);
 // вторая часть работы
 if(comm_rank == 0){ // определим призводительность каждого узла
  // В процессе работы предыдущей части мы собрали времена работы каждого узла (times) и количество элементов, которое было расчитано каждым узлом (partialsN).
  //double middlePartialN = 0; // среднее количество элементов (для случаев, когда число строк не делится на число процессов без остатка
  out("Гетрогенный случай; Процессов: %d\n", comm_size);
  global_start_time = MPI_Wtime(); // главный процесс запоминает время начала расчёта
  double timesSum = 0;
  for(int i = 0; i < comm_size; ++i){
   //middlePartialN += partialsN[i];
   times[i] = 1 / times[i]; // В формулах учавствует только величины, обратные времени расчёта
   timesSum += times[i];
  }
  //middlePartialN /= comm_size; // среднее число элементов
  int sumPartialsN = 0;
  for(int i = 0; i < comm_size; ++i){
   partialsN[i] = (int)llround(N * (times[i] / timesSum)); // cast long to int
   sumPartialsN += partialsN[i];
  }
  int index = 0;
  while(sumPartialsN != N){ // Ну как же без погрешностей. На случай, если розданное количество строк не равно количеству строк матрицы A
   if(sumPartialsN > N){ // если роздано больше строк, чем нужно
   if(partialsN[index] > 0){ // по не пустым процессам
    --partialsN[index];
    --sumPartialsN;
   }
  }else{ // если роздано меньше строк
   ++partialsN[index];
   ++sumPartialsN;
  }
  if(++index == comm_size){ index = 0; }
 }
 //out("%d %d\n", sumPartialsN, N);
 // теперь имеем правильно розданные строки матрицы между процессами, настало время послать дочерним новые данные для расчёта.
 /* План обмена данными
    главный процесс расчитывает количество элементов матрицы A, и посылает его дочерним процессам (6)
    дочерние процессы выделяют память для матриц A,B,C, учавствующих в расчёте.
    главный процесс посылает элементы матрицы A дочерним процессам (7)
    главный процесс рассылает всем дочерним процессам матрицу B
    все процессы ведут расчёт
    дочерние процессы посылают главному элементы матрицы C (8)
    дочерние процессы посылают главному время расчёта (9)
 */
  partialN = partialsN[0];
  index = partialN * L; // номер элемента это количество элементов будет считать процесс 0
  int partial_size;
  out("Процесс (0) расчитает строк матрицы A: %d\n", partialsN[0]);
  for(int i = 1; i < comm_size; ++i){
   partial_size = partialsN[i] * L;
   out("Процесс (%d) расчитает строк матрицы A: %d\n", i, partialsN[i]);
   MPI_Send(&partial_size, 1, MPI_INT, i, i + comm_size * 6, comm);
   MPI_Send(A + index, partial_size, MPI_DOUBLE, i, i + comm_size * 7, comm);
   index += partial_size;
  }
 }
 if(comm_rank > 0){ // Код на дочерних процессах
  free(matrix);
  MPI_Recv(&partialSize, 1, MPI_INT, 0, comm_rank + comm_size * 6, comm, &status);
  partialN = partialSize / L;
  matrix = (double*)malloc((partialSize + sizeB + partialSize) * sizeof(double));
  A = matrix;
  B = matrix + partialSize;
  C = B + sizeB;
  MPI_Recv(A, partialSize, MPI_DOUBLE, 0, comm_rank + comm_size * 7, comm, &status);
 }
 MPI_Bcast(B, sizeB, MPI_DOUBLE, 0, comm); // Все процессы получают матрицу B такую же, как у процесса 0

 // можно приступить к расчёту
 start_time = MPI_Wtime(); // запоминаем время начала расчёта
 mult(A, B, C, partialN, L); // частично умножаем матрицы
 end_time = MPI_Wtime(); // запоминаем время конца расчёта
 work_time = end_time - start_time; // получаем время конца расчёта

 MPI_Barrier(comm); // точка синхронизации после окончания расчёта

 if(comm_rank > 0){
  MPI_Send(C, partialSize, MPI_DOUBLE, 0, comm_rank + comm_size * 8, comm);
  MPI_Send(&work_time, 1, MPI_DOUBLE, 0, comm_rank + comm_size * 9, comm);
 }
 if(comm_rank == 0){
  int partial_size, index = partialN * L;
  times[0] = work_time;
  out("Получено время расчёта процесса (0) : %lf с.\n", work_time);
  for(int i = 1; i < comm_size; ++i){
   partial_size = partialsN[i] * L;
   MPI_Recv(C + index, partial_size, MPI_DOUBLE, i, i + comm_size * 8, comm, &status);
   MPI_Recv(times + i, 1, MPI_DOUBLE, i, i + comm_size * 9, comm, &status);
   index += partial_size;
   out("Получено время расчёта процесса (%d) : %lf с.\n", i, times[i]);
  }
 }

 MPI_Barrier(comm); // последняя точка синхронизации (Не нужна (?))

 if(comm_rank == 0){
  // printMatrix(C, "C", N, L);
  double normMatrix2 = norm(C, N, L); // считаем норму матрицы
  global_end_time = MPI_Wtime(); // глобальное время конца расчёта
  global_work_time = global_end_time - global_start_time;
  out("Количество строк матрицы A: %d\nНорма матрицы: %lf\nВремя расчёта: %lf с.\n", N, normMatrix2, global_work_time);
 }

 free(matrix); // освобождаем память
 MPI_Finalize(); // конец параллелизации
 return 0;
}
