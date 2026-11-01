#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <sys/wait.h>

int **createTheMatrix(int r, int c);
void freeMatrix(int **matrix, int r);
int **multiplication(int rA, int cA, int rB, int cB, int **matrixA, int **matrixB);
int **transpose(int r, int c, int **matrix);
double average(int r, int c, int **matrix);

int main(){

srand(time(NULL));

clock_t start;
clock_t end;
//clock_t overallStart;
//clock_t overallEnd;

double multTime;
double transposeTime;
double avgTime;
//double overallTime;

//overallStart = clock();

int rA = rand() % 10 + 2;
int cA = rand() % 10 + 2;

int rB = cA;
int cB = rand() % 10 + 2;

int **matrixA = createTheMatrix(rA, cA); 
int **matrixB = createTheMatrix(rB, cB);

int **multRes;

int **transRes;

double avgResult;

char choiceOne;
char choiceTwo;

pid_t firstChild;
pid_t secondChild;
pid_t thirdChild;


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

firstChild = fork();
if(firstChild == 0){
start = clock();

multRes = multiplication(rA, cA, rB, cB, matrixA, matrixB);

end = clock();
multTime = (double) (end - start) / CLOCKS_PER_SEC;

printf("=============================================================\n");

printf("Multiplication Result:\n");

for (int i = 0; i < rA; i++){
    
   for (int j = 0; j < cB; j++){
       
       printf("%d ", multRes[i][j]);
    }
    
    printf("\n");
}

printf("=============================================================\n");
printf("Multiplication Execution Time: %f seconds\n", multTime);

freeMatrix(matrixA, rA);
freeMatrix(matrixB, rB);
freeMatrix(multRes, rA);

exit(0);
}

secondChild = fork();
if(secondChild == 0){

start = clock();

if(choiceOne == 'A' || choiceOne == 'a'){

   transRes = transpose(rA, cA, matrixA);	
   
}else if(choiceOne == 'B' || choiceOne == 'b'){
  
   transRes = transpose(rB, cB, matrixB);	
   
}else{
  
  printf("Invalid Choice\n");
  return 1;
}

end = clock();

transposeTime = (double) (end - start) / CLOCKS_PER_SEC;

printf("=============================================================\n");

if(choiceOne == 'A' || choiceOne == 'a'){

   printf("Transpose of Matrix A:\n");

   for (int i = 0; i < cA; i++){
       
       for (int j = 0; j < rA; j++){
           
           printf("%d ", transRes[i][j]);
       }

       printf("\n");
    }
}else if(choiceOne == 'B' || choiceOne == 'b'){
  
   printf("Transpose of Matrix B:\n");

   for(int i = 0; i < cB; i++){
      
      for(int j = 0; j < rB; j++){
         
         printf("%d ", transRes[i][j]);
       }

       printf("\n");
    }
}

printf("=============================================================\n");
printf("Transpose Execution Time: %f seconds\n", transposeTime);

freeMatrix(matrixA, rA);
freeMatrix(matrixB, rB);
if (choiceOne == 'A' || choiceOne == 'a'){
    
    freeMatrix(transRes, cA);
}
else{
   
    freeMatrix(transRes, cB);
}

exit(0);
}

thirdChild = fork();
if(thirdChild == 0){
start = clock();

if(choiceTwo == 'A' || choiceTwo == 'a'){
  
   avgResult = average(rA, cA, matrixA);

}else if(choiceTwo == 'B' || choiceTwo == 'b'){
  
   avgResult = average(rB, cB, matrixB);

}else{
  printf("Invalid Choice\n");
  return 1;
}

end = clock();
avgTime = (double)(end - start) / CLOCKS_PER_SEC;

printf("=============================================================\n");

if(choiceTwo == 'A' || choiceTwo == 'a'){
   
   printf("Average of Matrix A is: %f\n", avgResult);

}else if(choiceTwo == 'B' || choiceTwo == 'b'){
 
   printf("Average of Matrix B is: %f\n", avgResult);
   
}else{
  printf("Invalid Choice\n");
  return 1;
}

printf("=============================================================\n");
printf("Average Execution Time: %f seconds\n", avgTime);

freeMatrix(matrixA, rA);
freeMatrix(matrixB, rB);

exit(0);
}

wait(NULL);
wait(NULL);
wait(NULL);

//overallEnd = clock();
//overallTime = (double)(overallEnd - overallStart) / CLOCKS_PER_SEC;

//printf("=============================================================\n");
//printf("Application Execution Time: %f seconds\n", overallTime);

freeMatrix(matrixA, rA);
freeMatrix(matrixB, rB);

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
