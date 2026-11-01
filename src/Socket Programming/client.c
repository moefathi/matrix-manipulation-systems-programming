#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <time.h>

int **createTheMatrix(int r, int c);
void freeMatrix(int **matrix, int r);


int main(int argc, char *argv[]){ 
int sockfd, portno, n;
struct sockaddr_in serv_addr;
// struct hostent *server;
char buffer[256];
portno = 1200;
sockfd = socket(AF_INET, SOCK_STREAM, 0); 
//server = gethostbyname(argv[1]);
bzero((char *) &serv_addr, sizeof(serv_addr));
serv_addr.sin_family = AF_INET;
//bcopy((char *)server->h_addr,(char *)&serv_addr.sin_addr.s_addr,server->h_length);
serv_addr.sin_addr.s_addr = INADDR_ANY;
serv_addr.sin_port = htons(portno);
connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr));
//senario

srand(time(NULL));

int rA = rand() % 10 + 2;
int cA = rand() % 10 + 2;

int rB = cA;
int cB = rand() % 10 + 2;

int **matrixA;
int **matrixB;
int **multRes;
int **transRes;

double avgResult;

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

write(sockfd, &rA, sizeof(int));
write(sockfd, &cA, sizeof(int));
write(sockfd, &rB, sizeof(int));
write(sockfd, &cB, sizeof(int));

for(int i = 0; i < rA; i++){

   write(sockfd, matrixA[i], cA * sizeof(int));
}

for(int i = 0; i < rB; i++){

   write(sockfd, matrixB[i], cB * sizeof(int));
}

write(sockfd, &choiceOne, sizeof(char));
write(sockfd, &choiceTwo, sizeof(char));

multRes = createTheMatrix(rA, cB);

for(int i = 0; i < rA; i++){

   read(sockfd, multRes[i], cB * sizeof(int));
}

if(choiceOne == 'A' || choiceOne == 'a'){

   transRes = createTheMatrix(cA, rA);

   for(int i = 0; i < cA; i++){

      read(sockfd, transRes[i], rA * sizeof(int));
   }

}else{

   transRes = createTheMatrix(cB, rB);

   for(int i = 0; i < cB; i++){

      read(sockfd, transRes[i], rB * sizeof(int));
   }
}

read(sockfd, &avgResult, sizeof(double));

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

freeMatrix(matrixA, rA);
freeMatrix(matrixB, rB);
freeMatrix(multRes, rA);

if(choiceOne == 'A' || choiceOne == 'a'){

   freeMatrix(transRes, cA);

}else{

   freeMatrix(transRes, cB);
}

 close(sockfd);
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