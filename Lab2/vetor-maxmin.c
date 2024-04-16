//Encontra o menor e o maior valor em um vetor de float
#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>

#define TESTE

//escopo global (par ser compartilhado com as threads)
float *vetor; //vetor de entrada

//define o tipo de dado de entrada das threads
typedef struct {
   int id; //id da thread
   long int tamBloco; //tamanho do bloco (cada thread processa um bloco)
} tArgs;

//define o tipo de retorno das threads
typedef struct {
   float max, min;
} tMaxMin;


//fluxo das threads
void * tarefa(void * arg) {
   tArgs *args = (tArgs *) arg; 

   tMaxMin *maxminLocal; //variavel local com os resultados da thread
   maxminLocal = (tMaxMin*) malloc(sizeof(tMaxMin));
   if(maxminLocal==NULL) {
      pthread_exit(NULL);
   }
   
   long int ini = args->id * args->tamBloco; //elemento inicial do bloco da thread
   long int fim = ini + args->tamBloco; //elemento final(nao processado) do bloco da thread
   //encontra o maior e menor elementos do bloco da thread
   maxminLocal->max = vetor[ini];
   maxminLocal->min = vetor[ini];
   for(long int i=ini+1; i<fim; i++) {
      if(vetor[i] > maxminLocal->max) 
         maxminLocal->max =vetor[i];
      if(vetor[i] < maxminLocal->min) 
         maxminLocal->min =vetor[i];
   }   

   //retorna o resultado da soma local
   pthread_exit((void *) maxminLocal); 
}

//fluxo principal
int main(int argc, char *argv[]) {
   float maiorGeral, menorGeral; //valores encontrados pela solucao concorrente
   long int dim; //dimensao do vetor de entrada
   long int tamBloco; //tamanho do bloco de cada thread 
   int nthreads; //numero de threads que serao criadas

   //tArgs *args; //vetor de argumentos para as threads
   pthread_t *tid; //vetor de identificadores das threads no sistema
   tMaxMin *retorno; //valor de retorno das threads

#ifdef TESTE
   float maiorCorreto, menorCorreto; //valores de referencia para os testes
#endif

   //recebe e valida os parametros de entrada (dimensao do vetor, numero de threads)
   if(argc < 2) {
       fprintf(stderr, "Digite: %s <numero threads>\n", argv[0]);
       return 1; 
   }
   nthreads = atoi(argv[1]);
   printf("nthreads=%d\n", nthreads); 

   //carrega o vetor de entrada
   scanf("%ld", &dim); //primeiro pergunta a dimensao do vetor
   printf("dim=%ld\n", dim); 

   //aloca o vetor de entrada
   vetor = (float*) malloc(sizeof(float)*dim);
   if(vetor == NULL) {
      fprintf(stderr, "ERRO--malloc\n");
      return 2;
   }
   //preenche o vetor de entrada
   for(long int i=0; i<dim; i++)
      scanf("%f", &vetor[i]);
#ifdef TESTE
   //le os valores esperados de maior e menor
   scanf("%f %f", &maiorCorreto, &menorCorreto);
   for(long int i=0; i<dim; i++)
      printf("%f ", vetor[i]); 
#endif   
   //inicializa as variaveis de saida com o valor na primeira posicao do vetor
   menorGeral = vetor[0];
   maiorGeral = vetor[0];

   //cria as threads
   tid = (pthread_t *) malloc(sizeof(pthread_t) * nthreads);
   if(tid==NULL) {
      fprintf(stderr, "ERRO--malloc\n");
      return 3;
   }
   tamBloco = dim/nthreads;  //tamanho do bloco de cada thread 
   if(!tamBloco) 
      printf("\nA quantidade de threads eh maior que a quantidade de elementos, a execucao sera sequencial!\n");
   for(int i=0; i<nthreads; i++) {
       //aloca e preenche argumentos para thread
       tArgs *args = (tArgs*) malloc(sizeof(tArgs));
       if (args == NULL) {
          printf("--ERRO: malloc()\n"); 
          pthread_exit(NULL);
       }
       args->id = i; 
       args->tamBloco = tamBloco; 
       if(pthread_create(tid+i, NULL, tarefa, (void*) args)){
          fprintf(stderr, "ERRO--pthread_create\n");
          return 5;
       }
   }
   
   //processa a parte final do vetor
   if(dim%nthreads) {
      puts("\nMain com tarefa");
      for(long int i=dim-(dim%nthreads); i<dim; i++) {
         if(vetor[i] > maiorGeral)
            maiorGeral = vetor[i];
         if(vetor[i] < menorGeral)
            menorGeral = vetor[i];
      }
   } 

   //aguarda o termino das threads
   for(int i=0; i<nthreads; i++) {
      if(pthread_join(*(tid+i), (void**) &retorno)){
         fprintf(stderr, "ERRO--pthread_create\n");
         return 6;
      }
      //encontrar o maior e menor valor entre os valores retornados pelas threads
      if (retorno->max > maiorGeral) maiorGeral = retorno->max;
      if (retorno->min < menorGeral) menorGeral = retorno->min;
   }

   //exibir os resultados
   printf("\n\nMaior valor:%f\nMenor valor:%f \n", maiorGeral, menorGeral);

#ifdef TESTE
   printf("\n\nMaior correto:%f\nMenor correto:%f \n", maiorCorreto, menorCorreto);
#endif

   //libera as areas de memoria alocadas
   free(vetor);
   free(tid);

   return 0;
}
