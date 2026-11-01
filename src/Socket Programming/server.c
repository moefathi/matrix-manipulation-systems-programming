#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define MAX_CLIENTS 100

int **createTheMatrix(int r, int c);
void freeMatrix(int **matrix, int r);
int **transpose(int r, int c, int **matrix);
void *clientThread(void *arg);
void *multiplicationThread(void *arg);
void *multiplicationSubThread(void *arg);
void *transposeThread(void *arg);
void *averageThread(void *arg);
void *averageSubThread(void *arg);

int rA[MAX_CLIENTS];
int cA[MAX_CLIENTS];
int rB[MAX_CLIENTS];
int cB[MAX_CLIENTS];

int **matrixA[MAX_CLIENTS];
int **matrixB[MAX_CLIENTS];

int **multRes[MAX_CLIENTS];
int **transRes[MAX_CLIENTS];

double avgResult[MAX_CLIENTS];

char choiceOne[MAX_CLIENTS];
char choiceTwo[MAX_CLIENTS];

int totalSum[MAX_CLIENTS];

int clientCount = 0;

pthread_mutex_t lock[MAX_CLIENTS];
pthread_mutex_t clientLock;
pthread_mutex_t logLock;

int main(int argc, char *argv[])
{
 int sockfd, portno, clilen;
 struct sockaddr_in serv_addr, cli_addr;
 pthread_t tid;

 sockfd = socket(AF_INET, SOCK_STREAM, 0);

 portno = 1200;
 serv_addr.sin_family = AF_INET;
 serv_addr.sin_port = htons(portno);
 serv_addr.sin_addr.s_addr = INADDR_ANY;

 bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr));

 listen(sockfd,5);

 pthread_mutex_init(&clientLock, NULL);
 pthread_mutex_init(&logLock, NULL);

 while(1){

  clilen = sizeof(cli_addr);

  int *newsockfd = malloc(sizeof(int));

  *newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);

  pthread_create(&tid, NULL, clientThread, newsockfd);

  pthread_detach(tid);
 }

 close(sockfd);

 return 0;
}

void *clientThread(void *arg){

 int newsockfd = *(int *)arg;

 free(arg);

 pthread_mutex_lock(&clientLock);

 int client = clientCount;
 clientCount++;

 pthread_mutex_unlock(&clientLock);

 char buffer[256];

 int logFile;

 sprintf(buffer, "Client %d Connected\n", client + 1);

 pthread_mutex_lock(&logLock);

 logFile = open("serverLog.txt", O_WRONLY | O_CREAT | O_APPEND, 0644);

 write(logFile, buffer, strlen(buffer));

 close(logFile);

 pthread_mutex_unlock(&logLock);

 read(newsockfd, &rA[client], sizeof(int));
 read(newsockfd, &cA[client], sizeof(int));
 read(newsockfd, &rB[client], sizeof(int));
 read(newsockfd, &cB[client], sizeof(int));

 matrixA[client] = createTheMatrix(rA[client], cA[client]);
 matrixB[client] = createTheMatrix(rB[client], cB[client]);

 for(int i = 0; i < rA[client]; i++){

  read(newsockfd, matrixA[client][i], cA[client] * sizeof(int));
 }

 for(int i = 0; i < rB[client]; i++){

  read(newsockfd, matrixB[client][i], cB[client] * sizeof(int));
 }

 read(newsockfd, &choiceOne[client], sizeof(char));
 read(newsockfd, &choiceTwo[client], sizeof(char));

 printf("=============================================================\n");
 printf("Client %d Matrix A (%d X %d)\n", client + 1, rA[client], cA[client]);

 for(int i = 0; i < rA[client]; i++){

  for(int j = 0; j < cA[client]; j++){

   printf("%d ", matrixA[client][i][j]);
  }

  printf("\n");
 }

 printf("=============================================================\n");
 printf("Client %d Matrix B (%d X %d)\n", client + 1, rB[client], cB[client]);

 for(int i = 0; i < rB[client]; i++){

  for(int j = 0; j < cB[client]; j++){

   printf("%d ", matrixB[client][i][j]);
  }

  printf("\n");
 }

 pthread_mutex_lock(&logLock);

 logFile = open("serverLog.txt", O_WRONLY | O_CREAT | O_APPEND, 0644);

 sprintf(buffer, "\nMatrix A (%d X %d)\n", rA[client], cA[client]);

 write(logFile, buffer, strlen(buffer));

 for(int i = 0; i < rA[client]; i++){

  for(int j = 0; j < cA[client]; j++){

   sprintf(buffer, "%d ", matrixA[client][i][j]);

   write(logFile, buffer, strlen(buffer));
  }

  write(logFile, "\n", 1);
 }

 sprintf(buffer, "\nMatrix B (%d X %d)\n", rB[client], cB[client]);

 write(logFile, buffer, strlen(buffer));

 for(int i = 0; i < rB[client]; i++){

  for(int j = 0; j < cB[client]; j++){

   sprintf(buffer, "%d ", matrixB[client][i][j]);

   write(logFile, buffer, strlen(buffer));
  }

  write(logFile, "\n", 1);
 }

 close(logFile);

 pthread_mutex_unlock(&logLock);

 pthread_mutex_init(&lock[client], NULL);

 pthread_t firstThread;
 pthread_t secondThread;
 pthread_t thirdThread;

 pthread_create(&firstThread, NULL, multiplicationThread, &client);
 pthread_create(&secondThread, NULL, transposeThread, &client);
 pthread_create(&thirdThread, NULL, averageThread, &client);

 pthread_join(firstThread, NULL);
 pthread_join(secondThread, NULL);
 pthread_join(thirdThread, NULL);

 pthread_mutex_lock(&logLock);

 logFile = open("serverLog.txt", O_WRONLY | O_CREAT | O_APPEND, 0644);

 sprintf(buffer, "\nMultiplication Result (%d X %d)\n", rA[client], cB[client]);

 write(logFile, buffer, strlen(buffer));

 for(int i = 0; i < rA[client]; i++){

  for(int j = 0; j < cB[client]; j++){

   sprintf(buffer, "%d ", multRes[client][i][j]);

   write(logFile, buffer, strlen(buffer));
  }

  write(logFile, "\n", 1);
 }

 if(choiceOne[client] == 'A' || choiceOne[client] == 'a'){

  sprintf(buffer, "\nTranspose Of Matrix A (%d X %d)\n", cA[client], rA[client]);

  write(logFile, buffer, strlen(buffer));

  for(int i = 0; i < cA[client]; i++){

   for(int j = 0; j < rA[client]; j++){

    sprintf(buffer, "%d ", transRes[client][i][j]);

    write(logFile, buffer, strlen(buffer));
   }

   write(logFile, "\n", 1);
  }

 }else{

  sprintf(buffer, "\nTranspose Of Matrix B (%d X %d)\n", cB[client], rB[client]);

  write(logFile, buffer, strlen(buffer));

  for(int i = 0; i < cB[client]; i++){

   for(int j = 0; j < rB[client]; j++){

    sprintf(buffer, "%d ", transRes[client][i][j]);

    write(logFile, buffer, strlen(buffer));
   }

   write(logFile, "\n", 1);
  }
 }

 if(choiceTwo[client] == 'A' || choiceTwo[client] == 'a'){

  sprintf(buffer, "\nAverage Of Matrix A = %.2f\n", avgResult[client]);

 }else{

  sprintf(buffer, "\nAverage Of Matrix B = %.2f\n", avgResult[client]);
 }

 write(logFile, buffer, strlen(buffer));

 close(logFile);

 pthread_mutex_unlock(&logLock);

 for(int i = 0; i < rA[client]; i++){

  write(newsockfd, multRes[client][i], cB[client] * sizeof(int));
 }

 if(choiceOne[client] == 'A' || choiceOne[client] == 'a'){

  for(int i = 0; i < cA[client]; i++){

   write(newsockfd, transRes[client][i], rA[client] * sizeof(int));
  }

 }else{

  for(int i = 0; i < cB[client]; i++){

   write(newsockfd, transRes[client][i], rB[client] * sizeof(int));
  }
 }

 write(newsockfd, &avgResult[client], sizeof(double));

 freeMatrix(matrixA[client], rA[client]);
 freeMatrix(matrixB[client], rB[client]);
 freeMatrix(multRes[client], rA[client]);

 if(choiceOne[client] == 'A' || choiceOne[client] == 'a'){

  freeMatrix(transRes[client], cA[client]);

 }else{

  freeMatrix(transRes[client], cB[client]);
 }

 pthread_mutex_destroy(&lock[client]);

 close(newsockfd);

 sprintf(buffer, "\nClient %d Disconnected\n", client + 1);

 pthread_mutex_lock(&logLock);

 logFile = open("serverLog.txt", O_WRONLY | O_CREAT | O_APPEND, 0644);

 write(logFile, buffer, strlen(buffer));

 strcpy(buffer, "=============================================================\n\n");

 write(logFile, buffer, strlen(buffer));

 close(logFile);

 pthread_mutex_unlock(&logLock);

 return NULL;
}

void *multiplicationThread(void *arg){

 int client = *((int *)arg);

 multRes[client] = createTheMatrix(rA[client], cB[client]);

 int threadsNum;

 if(rA[client] <= 10){

  threadsNum = rA[client];

 }else if(rA[client] <= 100){

  threadsNum = 10;

 }else if(rA[client] <= 1000){

  threadsNum = 20;

 }else{

  threadsNum = 100;
 }

 int rowsForThread = rA[client] / threadsNum;
 int extraRows = rA[client] % threadsNum;
 int current = 0;

 pthread_t threads[threadsNum];

 int threadData[threadsNum][3];

 for(int i = 0; i < threadsNum; i++){

  int rowsNow = rowsForThread;

  if(extraRows > 0){

   rowsNow++;
   extraRows--;
  }

  threadData[i][0] = client;
  threadData[i][1] = current;
  threadData[i][2] = rowsNow;

  pthread_create(&threads[i], NULL, multiplicationSubThread, threadData[i]);

  current += rowsNow;
 }

 for(int i = 0; i < threadsNum; i++){

  pthread_join(threads[i], NULL);
 }

 return NULL;
}

void *multiplicationSubThread(void *arg){

 int *data = (int *)arg;

 int client = data[0];
 int current = data[1];
 int rowsNow = data[2];

 for(int i = current; i < current + rowsNow; i++){

  for(int j = 0; j < cB[client]; j++){

   multRes[client][i][j] = 0;

   for(int k = 0; k < cA[client]; k++){

    multRes[client][i][j] += matrixA[client][i][k] * matrixB[client][k][j];
   }
  }
 }

 return NULL;
}

void *transposeThread(void *arg){

 int client = *((int *)arg);

 if(choiceOne[client] == 'A' || choiceOne[client] == 'a'){

  transRes[client] = transpose(rA[client], cA[client], matrixA[client]);

 }else{

  transRes[client] = transpose(rB[client], cB[client], matrixB[client]);
 }

 return NULL;
}

void *averageThread(void *arg){

 int client = *((int *)arg);

 int rows;
 int cols;

 if(choiceTwo[client] == 'A' || choiceTwo[client] == 'a'){

  rows = rA[client];
  cols = cA[client];

 }else{

  rows = rB[client];
  cols = cB[client];
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

 totalSum[client] = 0;

 pthread_t threads[threadsNum];

 int threadData[threadsNum][3];

 for(int i = 0; i < threadsNum; i++){

  int rowsNow = rowsForThread;

  if(extraRows > 0){

   rowsNow++;
   extraRows--;
  }

  threadData[i][0] = client;
  threadData[i][1] = current;
  threadData[i][2] = rowsNow;

  pthread_create(&threads[i], NULL, averageSubThread, threadData[i]);

  current += rowsNow;
 }

 for(int i = 0; i < threadsNum; i++){

  pthread_join(threads[i], NULL);
 }

 avgResult[client] = (double) totalSum[client] / (rows * cols);

 return NULL;
}

void *averageSubThread(void *arg){

 int *data = (int *)arg;

 int client = data[0];
 int current = data[1];
 int rowsNow = data[2];

 int **avgMatrix;
 int cols;

 if(choiceTwo[client] == 'A' || choiceTwo[client] == 'a'){

  avgMatrix = matrixA[client];
  cols = cA[client];

 }else{

  avgMatrix = matrixB[client];
  cols = cB[client];
 }

 int sum = 0;

 for(int i = current; i < current + rowsNow; i++){

  for(int j = 0; j < cols; j++){

   sum += avgMatrix[i][j];
  }
 }

 pthread_mutex_lock(&lock[client]);

 totalSum[client] += sum;

 pthread_mutex_unlock(&lock[client]);

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