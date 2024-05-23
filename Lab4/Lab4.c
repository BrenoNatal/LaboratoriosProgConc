/* Disciplina: Programacao Concorrente */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>
#include <time.h>
#include "timer.h"

int index = 0;         // variavel compartilhada entre as threads
pthread_mutex_t mutex; // variavel de lock para exclusao mutua

typedef struct
{
  int nThread;
  int N;
} t_args;

int ehPrimo(long long int n)
{
  int i;
  if (n <= 1)
    return 0;
  if (n == 2)
    return 1;
  if (n % 2 == 0)
    return 0;
  for (i = 3; i < sqrt(n) + 1; i += 2)
    if (n % i == 0)
      return 0;
  return 1;
}


// funcao executada pelas threads
void *ExecutaTarefa(void *param)
{
  t_args *args = (t_args *)param;

  int valor, resultado;

  //printf("Thread: %d esta executando...\n", args->nThread);
  while (index <= args->N)
  {
    pthread_mutex_lock(&mutex);
    valor = index;
    index++;
    pthread_mutex_unlock(&mutex);
    resultado = ehPrimo(valor);
    //printf("O valor da thread: %d é %d e retornou %d na funcao ehPrimo.\n", args->nThread, valor, resultado);
  }

  pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
  int nThreads;
  long long int N;
  pthread_t *tid;
  double inicio, fim, inicializacao, concorrente;

  if (argc < 2)
  {
    printf("Digite: %s <numero de threads> <numero de elementos> %d\n", argv[0], argc);
    return 1;
  }

  GET_TIME(inicio);

  nThreads = atoi(argv[1]);

  tid = (pthread_t *)malloc(sizeof(pthread_t) * nThreads);
  if (tid == NULL)
  {
    fprintf(stderr, "ERRO--malloc threads\n");
    return 2;
  }

  N = atoll(argv[2]);

  //--inicilaiza o mutex (lock de exclusao mutua)
  pthread_mutex_init(&mutex, NULL);

  GET_TIME(fim);
  inicializacao = fim - inicio;
  printf("%lf \n", inicializacao);

  GET_TIME(inicio);

  for (int i = 0; i < nThreads; i++)
  {
    t_args *args = (t_args *)malloc(sizeof(t_args));
    if (args == NULL)
    {
      printf("--ERRO: malloc()\n");
      pthread_exit(NULL);
    }

    args->nThread = i;
    args->N = N;

    if (pthread_create(tid + i, NULL, ExecutaTarefa, (void *)args))
    {
      printf("--ERRO: pthread_create()\n");
      exit(-1);
    }
  }

  //--espera todas as threads terminarem
  for (int i = 0; i < nThreads; i++)
  {
    if (pthread_join(*(tid + i), NULL))
    {
      printf("--ERRO: pthread_join() \n");
      exit(-1);
    }
  }
  pthread_mutex_destroy(&mutex);
  GET_TIME(fim);
  concorrente = fim - inicio;
  printf("%lf \n", concorrente);

}

