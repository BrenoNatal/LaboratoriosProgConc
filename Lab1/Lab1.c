#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

int *vetor;
int *vetorTeste;
int nThreads;

typedef struct
{
    long int idThread;
    int comeco, alcance;
} t_Args;

void *Elevar(void *arg)
{
    t_Args args = *(t_Args *)arg;
    for (int i = args.comeco; i < args.alcance; i++)
    {
        printf("Na thread %ld foi feito:\n %d * %d = %d \n",args.idThread, vetor[i], vetor[i], vetor[i] * vetor[i]);
        vetor[i] *= vetor[i];
    }

    pthread_exit(NULL);
}

void elevarSequencial(int tamanho)
{

    for (int i = 0; i < tamanho; i++)
    {
        vetorTeste[i] = vetor[i] * vetor[i];
    }

}

int teste(int tamanho)
{
    int erros = 0;

    for (int i = 0; i < tamanho; i++)
    {
        if (vetor[i] != vetorTeste[i]) 
        {
            erros ++;
            printf("Erro na posicao %d \n", i);
        }    
    }

    return erros;
}

void inicializarVetor(int tamanho)
{
    printf("Digite os elementos do vetor: ");
    for (int i = 0; i < tamanho; i++)
    {
        scanf("%d", &vetor[i]);
    }
}

void exibirVetor(int tamanho)
{
    printf("Vetor:\n");
    for (int i = 0; i < tamanho; i++)
    {
        printf("%d ", vetor[i]);
    }
    printf("\n");
}

int main()
{
    int tamanho, erros;
    t_Args *args;

    printf("Qual tamanho do vetor: ");
    scanf("%d", &tamanho);

    vetor = (int *)malloc(tamanho * sizeof(int));
    vetorTeste = (int *)malloc(tamanho * sizeof(int));

    if (vetor == NULL)
    {
        printf("Memoria insuficiente. \n");
        return 1;
    }
    if (vetorTeste == NULL)
    {
        printf("Memoria insuficiente vetor teste. \n");
        return 1;
    }


    inicializarVetor(tamanho);

    printf("Qual a quantidade de threads: ");
    scanf("%d", &nThreads);

    elevarSequencial(tamanho);

    pthread_t tid_sistema[nThreads];
    if(nThreads > tamanho){
        printf("So sera criado %d threads, devido ao tamanho do vetor ser menor que a quantidade de threads colocada. \n", tamanho);
        nThreads = tamanho;
    }

    for (long int i = 0; i < nThreads; i++)
    {
        args = malloc(sizeof(t_Args));
        if (args == NULL)
        {
            printf("--ERRO: malloc()\n");
            exit(-1);
        }
        args->idThread = i;
        if (tamanho <= nThreads) 
        {
            args->comeco = i;
            args->alcance = i+1;
        }
        else
        {
            if (i == nThreads - 1)
            {
                args->comeco = (tamanho / nThreads) * i;
                args->alcance = tamanho;
            }
            else
            {
                args->comeco = (tamanho / nThreads) * i;
                args->alcance = (tamanho / nThreads) * (i + 1);
            }    
        }
        printf("--Cria thread %ld\n", i);
        if (pthread_create(&tid_sistema[i], NULL, Elevar, (void *)args))
        {
            printf("--ERRO: pthread_create()\n");
            exit(-1);
        }
    }

    for (int i = 0; i < nThreads; i++)
    {
        if (pthread_join(tid_sistema[i], NULL))
        {
            printf("--ERRO: pthread_join() \n");
            exit(-1);
        }
    }

    printf("--Thread principal terminou\n");

    erros = teste(tamanho);

    printf("Teve %d erros\n", erros);

    exibirVetor(tamanho);
    free(vetor);
    free(vetorTeste);
    
    return 0;
}