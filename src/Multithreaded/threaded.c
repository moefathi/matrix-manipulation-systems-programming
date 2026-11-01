#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>

int **createTheMatrix(int r, int c);
void freeMatrix(int **matrix, int r);
int **transpose(int r, int c, int **matrix);
void *multiplicationThread(void *arg);
void *multiplicationSubThread(void *arg);
void *transposeThread(void *arg);
void *averageThread(void *arg);
void *averageSubThread(void *arg);

double multTime;
double transposeTime;
double avgTime;
double applicationTime;

int rA;
int cA;
int rB;
int cB;

int **matrixA;
int **matrixB;

int **multRes;
int **transRes;

double avgResult;

char choiceOne;
char choiceTwo;

int totalSum = 0;

pthread_mutex_t lock;

int main(){

srand(time(NULL));

clock_t overallStart = clock();

rA = rand() % 10 + 2;
cA = rand() % 10 + 2;

rB = cA;
cB = rand() % 10 + 2;

matrixA = createTheMatrix(rA, cA);
matrixB = createTheMatrix(rB, cB);

for(int i = 0; i < rA; i++){
   
   for(int j = 0; j < cA; j++){
   
      matrixA[i][j] = rand() % 9 + 1;
   }
}

printf("=============================================================\n");
printf("Matrix A (%d X %d)\n", rA, cA);

for(int i = 0; i < rA; i++){
   
   for(int j = 0; j < cA; j++){
   
      printf("%d ", matrixA[i][j]);
   }

   printf("\n");
}

for(int i = 0; i < rB; i++){
   
   for(int j = 0; j < cB; j++){
   
      matrixB[i][j] = rand() % 9 + 1;
   }
}

printf("=============================================================\n");
printf("Matrix B (%d X %d)\n", rB, cB);

for(int i = 0; i < rB; i++){
   
   for(int j = 0; j < cB; j++){
   
      printf("%d ", matrixB[i][j]);
   }

   printf("\n");
}

printf("=============================================================\n");

printf("Which Matrix Do You Want To Transpose? (A/B)\n");
scanf(" %c", &choiceOne);

printf("=============================================================\n");

printf("Which Matrix Do You Want To Calculate Average For? (A/B)\n");
scanf(" %c", &choiceTwo);

pthread_mutex_init(&lock, NULL);

pthread_t firstThread;
pthread_t secondThread;
pthread_t thirdThread;

pthread_create(&firstThread, NULL, multiplicationThread, NULL);
pthread_create(&secondThread, NULL, transposeThread, NULL);
pthread_create(&thirdThread, NULL, averageThread, NULL);

pthread_join(firstThread, NULL);
pthread_join(secondThread, NULL);
pthread_join(thirdThread, NULL);

printf("=============================================================\n");
printf("Multiplication Result (%d X %d)\n", rA, cB);

for(int i = 0; i < rA; i++){

   for(int j = 0; j < cB; j++){

      printf("%d ", multRes[i][j]);
   }

   printf("\n");
}

printf("=============================================================\n");

if(choiceOne == 'A' || choiceOne == 'a'){

   printf("Transpose Of Matrix A (%d X %d)\n", cA, rA);

   for(int i = 0; i < cA; i++){

      for(int j = 0; j < rA; j++){

         printf("%d ", transRes[i][j]);
      }

      printf("\n");
   }

}else{

   printf("Transpose Of Matrix B (%d X %d)\n", cB, rB);

   for(int i = 0; i < cB; i++){

      for(int j = 0; j < rB; j++){

         printf("%d ", transRes[i][j]);
      }

      printf("\n");
   }
}

printf("=============================================================\n");

if(choiceTwo == 'A' || choiceTwo == 'a'){

   printf("Average Of Matrix A = %.2f\n", avgResult);

}else{

   printf("Average Of Matrix B = %.2f\n", avgResult);
}

clock_t overallEnd = clock();

applicationTime = (double)(overallEnd - overallStart) / CLOCKS_PER_SEC;

printf("=============================================================\n");
printf("Multiplication Time = %f seconds\n", multTime);
printf("Transpose Time = %f seconds\n", transposeTime);
printf("Average Time = %f seconds\n", avgTime);
printf("Application Time = %f seconds\n", applicationTime);

freeMatrix(matrixA, rA);
freeMatrix(matrixB, rB);
freeMatrix(multRes, rA);

if(choiceOne == 'A' || choiceOne == 'a'){

   freeMatrix(transRes, cA);

}else{

   freeMatrix(transRes, cB);
}

pthread_mutex_destroy(&lock);

return 0;
}

void *multiplicationThread(void *arg){

   clock_t start;
   clock_t end;
   start = clock();

   multRes = createTheMatrix(rA, cB);

   int threadsNum;

   if(rA <= 10){

      threadsNum = rA;

   }else if(rA <= 100){

      threadsNum = 10;

   }else if(rA <= 1000){

      threadsNum = 20;

   }else{

      threadsNum = 100;
   }

   int rowsForThread = rA / threadsNum;
   int extraRows = rA % threadsNum;
   int current = 0;

   pthread_t threads[threadsNum];

   int threadData[threadsNum][2];

   for(int i = 0; i < threadsNum; i++){

      int rowsNow = rowsForThread;

      if(extraRows > 0){

         rowsNow++;
         extraRows--;
      }

      threadData[i][0] = current;
      threadData[i][1] = rowsNow;

      pthread_create(&threads[i], NULL, multiplicationSubThread, threadData[i]);

      current += rowsNow;
   }

   for(int i = 0; i < threadsNum; i++){

      pthread_join(threads[i], NULL);
   }
   end = clock();
   multTime = (double)(end - start) / CLOCKS_PER_SEC;

   return NULL;
}

void *multiplicationSubThread(void *arg){

   int *data = (int *)arg;

   int current = data[0];
   int rowsNow = data[1];

   for(int i = current; i < current + rowsNow; i++){

      for(int j = 0; j < cB; j++){

         multRes[i][j] = 0;

         for(int k = 0; k < cA; k++){

            multRes[i][j] += matrixA[i][k] * matrixB[k][j];
         }
      }
   }

   return NULL;
}

void *transposeThread(void *arg){
   
   clock_t start;
   clock_t end;
   start = clock();

   if(choiceOne == 'A' || choiceOne == 'a'){

      transRes = transpose(rA, cA, matrixA);

   }else{

      transRes = transpose(rB, cB, matrixB);
   }

   end = clock();
   transposeTime = (double)(end - start) / CLOCKS_PER_SEC;

   return NULL;
}

void *averageThread(void *arg){

   clock_t start;
   clock_t end;
   start = clock();

   int rows;
   int cols;

   if(choiceTwo == 'A' || choiceTwo == 'a'){

      rows = rA;
      cols = cA;

   }else{

      rows = rB;
      cols = cB;
   }

   int threadsNum;

   if(rows <= 10){

      threadsNum = rows;

   }else if(rows <= 100){

      threadsNum = 10;

   }else if(rows <= 1000){

      threadsNum = 20;

   }else{

      threadsNum = 100;
   }

   int rowsForThread = rows / threadsNum;
   int extraRows = rows % threadsNum;
   int current = 0;

   totalSum = 0;

   pthread_t threads[threadsNum];

   int threadData[threadsNum][2];

   for(int i = 0; i < threadsNum; i++){

      int rowsNow = rowsForThread;

      if(extraRows > 0){

         rowsNow++;
         extraRows--;
      }

      threadData[i][0] = current;
      threadData[i][1] = rowsNow;

      pthread_create(&threads[i], NULL, averageSubThread, threadData[i]);

      current += rowsNow;
   }

   for(int i = 0; i < threadsNum; i++){

      pthread_join(threads[i], NULL);
   }

   avgResult = (double) totalSum / (rows * cols);

   end = clock();
   avgTime = (double)(end - start) / CLOCKS_PER_SEC;

   return NULL;
}

void *averageSubThread(void *arg){

   int *data = (int *)arg;

   int current = data[0];
   int rowsNow = data[1];

   int **avgMatrix;
   int cols;

   if(choiceTwo == 'A' || choiceTwo == 'a'){

      avgMatrix = matrixA;
      cols = cA;

   }else{

      avgMatrix = matrixB;
      cols = cB;
   }

   int sum = 0;

   for(int i = current; i < current + rowsNow; i++){

      for(int j = 0; j < cols; j++){

         sum += avgMatrix[i][j];
      }
   }

   pthread_mutex_lock(&lock);

   totalSum += sum;

   pthread_mutex_unlock(&lock);

   return NULL;
}

int **createTheMatrix(int r, int c){
   
   int **matrix = malloc(r * sizeof(int *));
   
   for(int i = 0; i < r; i++){
      
      matrix[i] = malloc(c * sizeof(int));
      
      }
   return matrix;
   }
   
void freeMatrix(int **matrix, int r){

   for(int i = 0; i < r; i++){
    
      free(matrix[i]);
   }
   
   free(matrix);
}

int **transpose(int r, int c, int **matrix){
  
   int **res = createTheMatrix(c, r);
   
   for(int i = 0; i < r; i++){
   
       for(int j = 0; j < c; j++){
          
          res[j][i] = matrix[i][j];
          
       }
   }
   
   return res;
}