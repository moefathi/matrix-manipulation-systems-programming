#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

int **createTheMatrix(int r, int c);
void freeMatrix(int **matrix, int r);
int **multiplication(int rA, int cA, int rB, int cB, int **matrixA, int **matrixB);
int **transpose(int r, int c, int **matrix);
double average(int r, int c, int **matrix);

int main(){

srand(time(NULL));

clock_t overallStart = clock();

pid_t firstChild;
pid_t secondChild;
pid_t thirdChild;

int parentToMult[2];
int multToParent[2];
int parentToAvg[2];
int avgToParent[2];
int parentToTrans[2];
int transToParent[2];

int multTimePipe[2];
int transTimePipe[2];
int avgTimePipe[2];

pipe(parentToMult);
pipe(multToParent);
pipe(parentToAvg);
pipe(avgToParent);
pipe(parentToTrans);
pipe(transToParent);

pipe(multTimePipe);
pipe(transTimePipe);
pipe(avgTimePipe);


//MULT PROCESS
firstChild = fork();
if(firstChild == 0){
  
   clock_t start;
   clock_t end;
   double multTime;
   int rA;
   int cA;
   int rB;
   int cB;
   int **matrixA;
   int **matrixB;

   start = clock();
   multTime = 0;
   
   close(parentToMult[1]);
   close(multTimePipe[0]);

   read(parentToMult[0], &rA, sizeof(int));
   read(parentToMult[0], &cA, sizeof(int));
   read(parentToMult[0], &rB, sizeof(int));
   read(parentToMult[0], &cB, sizeof(int));

   matrixA = createTheMatrix(rA, cA);
   matrixB = createTheMatrix(rB, cB);

   for(int i = 0; i < rA; i++){

      read(parentToMult[0], matrixA[i], cA * sizeof(int));
   }

   for(int i = 0; i < rB; i++){

      read(parentToMult[0], matrixB[i], cB * sizeof(int));
   }

   close(parentToMult[0]);

   int processesNum;

   if(rA <= 10){

      processesNum = rA;

   }else if(rA <= 100){

      processesNum = 10;

   }else if(rA <= 1000){

      processesNum = 20;

   }else{

      processesNum = 100;
   }
   
   int rowsForProcess = rA / processesNum;
   int extraRows = rA % processesNum;
   int current = 0;

   int childToMult[processesNum][2];
   int multSubTimePipe[2];

   pipe(multSubTimePipe);

   for(int i = 0; i < processesNum; i++){

      pipe(childToMult[i]);
   }

   pid_t multChild;

   for(int i = 0; i < processesNum; i++){

      int rowsNow = rowsForProcess;

      if(extraRows > 0){

         rowsNow++;
         extraRows--;
      }
      
      //MULT SUB-PROCESS
      multChild = fork();

      if(multChild == 0){

         clock_t start;
         clock_t end;
         double time;

         start = clock();

         close(childToMult[i][0]);
         close(multSubTimePipe[0]);

         int **childRes = createTheMatrix(rowsNow, cB);

         for(int j = 0; j < rowsNow; j++){

            for(int k = 0; k < cB; k++){

               childRes[j][k] = 0;

               for(int x = 0; x < cA; x++){

                  childRes[j][k] += matrixA[current + j][x] * matrixB[x][k];
               }
            }
         }

         write(childToMult[i][1], &current, sizeof(int));
         write(childToMult[i][1], &rowsNow, sizeof(int));

         for(int j = 0; j < rowsNow; j++){

            write(childToMult[i][1], childRes[j], cB * sizeof(int));
         }

         close(childToMult[i][1]);

         freeMatrix(childRes, rowsNow);

         end = clock();

         time = (double)(end - start) / CLOCKS_PER_SEC;

         write(multSubTimePipe[1], &time, sizeof(double));

         close(multSubTimePipe[1]);

         exit(0);
      }

      current += rowsNow;
   }

   for(int i = 0; i < processesNum; i++){

      close(childToMult[i][1]);
   }

   close(multSubTimePipe[1]);

   int **multRes = createTheMatrix(rA, cB);

   for(int i = 0; i < processesNum; i++){

      int start;
      int rows;

      read(childToMult[i][0], &start, sizeof(int));
      read(childToMult[i][0], &rows, sizeof(int));

      for(int j = start; j < start + rows; j++){

         read(childToMult[i][0], multRes[j], cB * sizeof(int));
      }

      close(childToMult[i][0]);
   }

   for(int i = 0; i < processesNum; i++){

      double time;

      read(multSubTimePipe[0], &time, sizeof(double));

      if(time > multTime){

         multTime = time;
      }
   }

   close(multSubTimePipe[0]);

   for(int i = 0; i < processesNum; i++){

      wait(NULL);
   }

   close(multToParent[0]);

   for(int i = 0; i < rA; i++){

      write(multToParent[1], multRes[i], cB * sizeof(int));
   }

   close(multToParent[1]);
   
   int namedPipeWrite;

   namedPipeWrite = open("mult_transpose", O_WRONLY);

   write(namedPipeWrite, &rA, sizeof(int));
   write(namedPipeWrite, &cB, sizeof(int));

   for(int i = 0; i < rA; i++){

      write(namedPipeWrite, multRes[i], cB * sizeof(int));
   }

   close(namedPipeWrite);

   freeMatrix(multRes, rA);
   freeMatrix(matrixA, rA);
   freeMatrix(matrixB, rB);

   end = clock();

   multTime += (double)(end - start) / CLOCKS_PER_SEC;

   write(multTimePipe[1], &multTime, sizeof(double));

   close(multTimePipe[1]);

   exit(0);
}


//TRANSPOSE PROCESS
secondChild = fork();
if(secondChild == 0){

   clock_t start;
   clock_t end;
   double transposeTime;

   start = clock();

   int rows;
   int cols;

   close(parentToTrans[1]);
   close(transTimePipe[0]);

   read(parentToTrans[0], &rows, sizeof(int));
   read(parentToTrans[0], &cols, sizeof(int));

   int **transMatrix = createTheMatrix(rows, cols);

   for(int i = 0; i < rows; i++){

      read(parentToTrans[0], transMatrix[i], cols * sizeof(int));
   }

   close(parentToTrans[0]);
   
   int **transRes = transpose(rows, cols, transMatrix);

   close(transToParent[0]);

   for(int i = 0; i < cols; i++){

      write(transToParent[1], transRes[i], rows * sizeof(int));
   }

   int namedPipeRead;

   namedPipeRead = open("mult_transpose", O_RDONLY);

   int multRows;
   int multCols;

   read(namedPipeRead, &multRows, sizeof(int));
   read(namedPipeRead, &multCols, sizeof(int));

   int **multMatrix = createTheMatrix(multRows, multCols);

   for(int i = 0; i < multRows; i++){

      read(namedPipeRead, multMatrix[i], multCols * sizeof(int));
   }

   close(namedPipeRead);

   int **multTransRes = transpose(multRows, multCols, multMatrix);

   multTransRes = realloc(multTransRes, (multCols + 1) * sizeof(int *));

   multTransRes[multCols] = malloc(multRows * sizeof(int));

   for(int i = 0; i < multRows; i++){

      multTransRes[multCols][i] = rand() % 9 + 1;
   }

   for(int i = 0; i < multCols + 1; i++){

      write(transToParent[1], multTransRes[i], multRows * sizeof(int));
   }

   close(transToParent[1]);

   freeMatrix(transRes, cols);
   freeMatrix(transMatrix, rows);
   freeMatrix(multMatrix, multRows);
   freeMatrix(multTransRes, multCols + 1);

   end = clock();

   transposeTime = (double)(end - start) / CLOCKS_PER_SEC;

   write(transTimePipe[1], &transposeTime, sizeof(double));

   close(transTimePipe[1]);

   exit(0);
}


//AVERAGE PROCESS
thirdChild = fork();
if(thirdChild == 0){

   clock_t start;
   clock_t end;
   double avgTime;

   start = clock();
   avgTime = 0;

   int rows;
   int cols;

   close(parentToAvg[1]);
   close(avgTimePipe[0]);

   read(parentToAvg[0], &rows, sizeof(int));
   read(parentToAvg[0], &cols, sizeof(int));

   int **avgMatrix = createTheMatrix(rows, cols);

   for(int i = 0; i < rows; i++){

      read(parentToAvg[0], avgMatrix[i], cols * sizeof(int));
   }

   close(parentToAvg[0]);

   int processesNum;

   if(rows <= 10){

      processesNum = rows;

   }else if(rows <= 100){

      processesNum = 10;

   }else if(rows <= 1000){

      processesNum = 20;

   }else{

      processesNum = 100;
   }

   int rowsForProcess = rows / processesNum;
   int extraRows = rows % processesNum;
   int current = 0;

   int childToAvg[processesNum][2];
   int avgSubTimePipe[2];

   pipe(avgSubTimePipe);

   for(int i = 0; i < processesNum; i++){

      pipe(childToAvg[i]);
   }

   pid_t avgChild;

   for(int i = 0; i < processesNum; i++){

      int rowsNow = rowsForProcess;

      if(extraRows > 0){

         rowsNow++;
         extraRows--;
      }

      //AVERAGE SUB-PROCESS
      avgChild = fork();

      if(avgChild == 0){

         clock_t start;
         clock_t end;
         double time;

         start = clock();

         close(childToAvg[i][0]);
         close(avgSubTimePipe[0]);

         int sum = 0;

         for(int j = current; j < current + rowsNow; j++){

            for(int k = 0; k < cols; k++){

               sum += avgMatrix[j][k];
            }
         }

         write(childToAvg[i][1], &sum, sizeof(int));

         close(childToAvg[i][1]);

         end = clock();

         time = (double)(end - start) / CLOCKS_PER_SEC;

         write(avgSubTimePipe[1], &time, sizeof(double));

         close(avgSubTimePipe[1]);

         exit(0);
      }

      current += rowsNow;
   }

   for(int i = 0; i < processesNum; i++){

      close(childToAvg[i][1]);
   }

   close(avgSubTimePipe[1]);

   int totalSum = 0;

   for(int i = 0; i < processesNum; i++){

      int sum;

      read(childToAvg[i][0], &sum, sizeof(int));

      totalSum += sum;

      close(childToAvg[i][0]);
   }

   for(int i = 0; i < processesNum; i++){

      double time;

      read(avgSubTimePipe[0], &time, sizeof(double));

      if(time > avgTime){

         avgTime = time;
      }
   }

   close(avgSubTimePipe[0]);

   for(int i = 0; i < processesNum; i++){

      wait(NULL);
   }

   double avgResult = (double) totalSum / (rows * cols);

   close(avgToParent[0]);

   write(avgToParent[1], &avgResult, sizeof(double));

   close(avgToParent[1]);

   freeMatrix(avgMatrix, rows);

   end = clock();

   avgTime += (double)(end - start) / CLOCKS_PER_SEC;

   write(avgTimePipe[1], &avgTime, sizeof(double));

   close(avgTimePipe[1]);

   exit(0);
}

//PARENT
close(multTimePipe[1]);
close(transTimePipe[1]);
close(avgTimePipe[1]);

int rA = rand() % 10 + 2;
int cA = rand() % 10 + 2;
int rB = cA;
int cB = rand() % 10 + 2;
int **matrixA;
int **matrixB;

int **multRes;
int **transRes;
int **multTransRes;
double avgResult;

double multTime;
double transposeTime;
double avgTime;
double applicationTime;

char choiceOne;
char choiceTwo;

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

close(parentToMult[0]);

write(parentToMult[1], &rA, sizeof(int));
write(parentToMult[1], &cA, sizeof(int));
write(parentToMult[1], &rB, sizeof(int));
write(parentToMult[1], &cB, sizeof(int));

for(int i = 0; i < rA; i++){

   write(parentToMult[1], matrixA[i], cA * sizeof(int));
}

for(int i = 0; i < rB; i++){

   write(parentToMult[1], matrixB[i], cB * sizeof(int));
}

close(parentToMult[1]);

close(parentToTrans[0]);

if(choiceOne == 'A' || choiceOne == 'a'){

   write(parentToTrans[1], &rA, sizeof(int));
   write(parentToTrans[1], &cA, sizeof(int));

   for(int i = 0; i < rA; i++){

      write(parentToTrans[1], matrixA[i], cA * sizeof(int));
   }

}else{

   write(parentToTrans[1], &rB, sizeof(int));
   write(parentToTrans[1], &cB, sizeof(int));

   for(int i = 0; i < rB; i++){

      write(parentToTrans[1], matrixB[i], cB * sizeof(int));
   }
}

close(parentToTrans[1]);

close(parentToAvg[0]);

if(choiceTwo == 'A' || choiceTwo == 'a'){

   write(parentToAvg[1], &rA, sizeof(int));
   write(parentToAvg[1], &cA, sizeof(int));

   for(int i = 0; i < rA; i++){

      write(parentToAvg[1], matrixA[i], cA * sizeof(int));
   }

}else{

   write(parentToAvg[1], &rB, sizeof(int));
   write(parentToAvg[1], &cB, sizeof(int));

   for(int i = 0; i < rB; i++){

      write(parentToAvg[1], matrixB[i], cB * sizeof(int));
   }
}

close(parentToAvg[1]);

close(multToParent[1]);

multRes = createTheMatrix(rA, cB);

for(int i = 0; i < rA; i++){

   read(multToParent[0], multRes[i], cB * sizeof(int));
}

close(multToParent[0]);

printf("=============================================================\n");
printf("Multiplication Result (%d X %d)\n", rA, cB);

for(int i = 0; i < rA; i++){

   for(int j = 0; j < cB; j++){

      printf("%d ", multRes[i][j]);
   }

   printf("\n");
}

close(transToParent[1]);

if(choiceOne == 'A' || choiceOne == 'a'){

   transRes = createTheMatrix(cA, rA);

   for(int i = 0; i < cA; i++){

      read(transToParent[0], transRes[i], rA * sizeof(int));
   }

}else{

   transRes = createTheMatrix(cB, rB);

   for(int i = 0; i < cB; i++){

      read(transToParent[0], transRes[i], rB * sizeof(int));
   }
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

multTransRes = createTheMatrix(cB + 1, rA);

for(int i = 0; i < cB + 1; i++){

   read(transToParent[0], multTransRes[i], rA * sizeof(int));
}

close(transToParent[0]);

printf("=============================================================\n");
printf("Transpose Of Multiplication Result With New Row (%d X %d)\n", cB + 1, rA);

for(int i = 0; i < cB + 1; i++){

   for(int j = 0; j < rA; j++){

      printf("%d ", multTransRes[i][j]);
   }

   printf("\n");
}

close(avgToParent[1]);

read(avgToParent[0], &avgResult, sizeof(double));

close(avgToParent[0]);

printf("=============================================================\n");

if(choiceTwo == 'A' || choiceTwo == 'a'){

   printf("Average Of Matrix A = %.f\n", avgResult);

}else{

   printf("Average Of Matrix B = %.f\n", avgResult);
}

read(multTimePipe[0], &multTime, sizeof(double));
read(transTimePipe[0], &transposeTime, sizeof(double));
read(avgTimePipe[0], &avgTime, sizeof(double));

close(multTimePipe[0]);
close(transTimePipe[0]);
close(avgTimePipe[0]);

printf("=============================================================\n");
printf("Multiplication Time = %.f seconds\n", multTime);
printf("Transpose Time = %.f seconds\n", transposeTime);
printf("Average Time = %.f seconds\n", avgTime);

wait(NULL);
wait(NULL);
wait(NULL);

freeMatrix(matrixA, rA);
freeMatrix(matrixB, rB);
freeMatrix(multRes, rA);

if(choiceOne == 'A' || choiceOne == 'a'){

   freeMatrix(transRes, cA);

}else{

   freeMatrix(transRes, cB);
}

freeMatrix(multTransRes, cB + 1);

clock_t overallEnd = clock();

applicationTime = (double)(overallEnd - overallStart) / CLOCKS_PER_SEC;

if(multTime >= transposeTime && multTime >= avgTime){

   applicationTime += multTime;

}else if(transposeTime >= multTime && transposeTime >= avgTime){

   applicationTime += transposeTime;

}else{

   applicationTime += avgTime;
}

printf("=============================================================\n");
printf("Application Time = %.f seconds\n", applicationTime);

return 0;
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

int **multiplication(int rA, int cA, int rB, int cB, int **matrixA, int **matrixB){
  
   if(cA != rB){
   
   return NULL;
   }
   
   int **res = createTheMatrix(rA, cB);
   
   for(int i = 0; i < rA; i++){
      
      for(int j = 0; j < cB; j++){
         
         res[i][j] = 0;
         
         for(int k = 0; k < cA; k++){
 	    
 	    res[i][j] += matrixA[i][k] * matrixB[k][j];        
         }
      }
   }
   
   return res;
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

double average(int r, int c, int **matrix){
   
   int sum = 0;
   int count = r * c;
   double avg = 0;
   
   for(int i = 0; i < r; i++){
   
      for(int j = 0; j < c; j++){
         
         sum += matrix[i][j];
      }
   }
   
   avg = (double) sum / count;
   return avg;
}
