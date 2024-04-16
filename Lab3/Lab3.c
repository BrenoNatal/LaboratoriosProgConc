#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "timer.h"
#include <pthread.h>

float *matrizA;             // Matriz de entrada A
float *matrizB;             // Matriz de entrada B
float *matrizC;             // Matriz de saida C
int linA, colA, linB, colB; // Dimensões das matrizes de entrada

#define SEQUECIAL

typedef struct Lab3
{
    int tamanho, nThread;

} t_args;

void *multiMatriz(void *arg)
{

    t_args *args = (t_args *)arg;

    if (args->tamanho)
    {
        long int inicio = args->nThread * args->tamanho;
        long int fim = inicio + args->tamanho;

        for (int i = inicio; i < fim; i++)
        {
            for (int j = 0; j < colB; j++)
            {
                matrizC[i * colB + j] = 0;
                for (int k = 0; k < colA; k++)
                {
                    matrizC[i * colB + j] += matrizA[i * colA + k] * matrizB[k * colB + j];
                }
            }
        }
    }

    pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
    double inicio, fim, inicializacao, final, concorrente;
    int nThreads; // Quantidade de Threads
    int tamBloco;
    long long int tamA, tamB, tamC; // Tamanho da matriz A
    pthread_t *tid;                 // identificadores das threads no sistema
    FILE *descritorArquivo;         // descritor do arquivo de entrada
    size_t ret;                     // retorno da funcao de leitura no arquivo de entrada

    if (argc < 5)
    {
        printf("Digite: %s <numero de threads> <arquivo da matriz A> <arquivo da matriz B> <arquivo de saida concorrente>\n", argv[0]);
        return 1;
    }

    // Inicializacao

    GET_TIME(inicio);

    nThreads = atoi(argv[1]);

    // abre o arquivo para leitura binaria
    descritorArquivo = fopen(argv[2], "rb");
    if (!descritorArquivo)
    {
        fprintf(stderr, "Erro de abertura do arquivo da matriz A\n");
        return 2;
    }

    // le as dimensoes da matriz A
    ret = fread(&linA, sizeof(int), 1, descritorArquivo);
    if (!ret)
    {
        fprintf(stderr, "Erro de leitura das dimensoes da matriz A \n");
        return 3;
    }
    ret = fread(&colA, sizeof(int), 1, descritorArquivo);
    if (!ret)
    {
        fprintf(stderr, "Erro de leitura das dimensoes da matriz arquivo \n");
        return 3;
    }
    tamA = linA * colA; // calcula a qtde de elementos da matriz

    // aloca memoria para a matriz A
    matrizA = (float *)malloc(sizeof(float) * tamA);
    if (!matrizA)
    {
        fprintf(stderr, "Erro de alocao da memoria da matriz\n");
        return 3;
    }

    // carrega a matriz de elementos do tipo float do arquivo
    ret = fread(matrizA, sizeof(float), tamA, descritorArquivo);
    if (ret < tamA)
    {
        fprintf(stderr, "Erro de leitura dos elementos da matriz\n");
        return 4;
    }

    // abre o arquivo para leitura binaria da matriz B
    descritorArquivo = fopen(argv[3], "rb");
    if (!descritorArquivo)
    {
        fprintf(stderr, "Erro de abertura do arquivo da matriz B\n");
        return 2;
    }

    // le as dimensoes da matriz B
    ret = fread(&linB, sizeof(int), 1, descritorArquivo);
    if (!ret)
    {
        fprintf(stderr, "Erro de leitura das dimensoes da matriz A \n");
        return 3;
    }
    ret = fread(&colB, sizeof(int), 1, descritorArquivo);
    if (!ret)
    {
        fprintf(stderr, "Erro de leitura das dimensoes da matriz arquivo \n");
        return 3;
    }
    tamB = linB * colB; // calcula a qtde de elementos da matriz

    // checa se o numero de colunas de A e numero de linhas de B eh igual
    if (colA != linB)
    {
        printf("Erro: Numero de colunas de A eh diferente de numero de linhas de B\n");
        return 8;
    }

    // aloca memoria para a matriz B
    matrizB = (float *)malloc(sizeof(float) * tamB);
    if (!matrizB)
    {
        fprintf(stderr, "Erro de alocao dinicializacao memoria da matriz B\n");
        return 3;
    }

    // carrega a matriz de elementos do tipo float do arquivo
    ret = fread(matrizB, sizeof(float), tamB, descritorArquivo);
    if (ret < tamB)
    {
        fprintf(stderr, "Erro de leitura dos elementos da matriz B\n");
        return 4;
    }

    // aloca memoria para a matriz C
    tamC = linA * colB;
    matrizC = (float *)malloc(sizeof(float) * tamC);
    if (!matrizC)
    {
        fprintf(stderr, "Erro de alocao da memoria da matriz C\n");
        return 3;
    }

    // Fim inicializacao

    GET_TIME(fim);
    inicializacao = fim - inicio;
    printf("%lf \n", inicializacao);

    GET_TIME(inicio);
    tid = (pthread_t *)malloc(sizeof(pthread_t) * nThreads);
    if (tid == NULL)
    {
        fprintf(stderr, "ERRO--malloc threads\n");
        return 5;
    }

    tamBloco = linA / nThreads;

    if (!tamBloco)
    {
        printf("\nA quantidade de threads eh maior que a quantidade de linhas, a execucao sera sequencial!\n");
    }

    // Comeco concorrente
    for (int i = 0; i < nThreads; i++)
    {

        t_args *args = (t_args *)malloc(sizeof(t_args));
        if (args == NULL)
        {
            printf("--ERRO: malloc()\n");
            pthread_exit(NULL);
        }

        args->nThread = i;
        args->tamanho = tamBloco;

        if (pthread_create(tid + i, NULL, multiMatriz, (void *)args))
        {
            fprintf(stderr, "ERRO--pthread_create\n");
            return 6;
        }
    }

    if (linA % nThreads)
    {
        for (int i = linA - (linA % nThreads); i < linA; i++)
        {
            for (int j = 0; j < colB; j++)
            {
                matrizC[i * colB + j] = 0;
                for (int k = 0; k < colA; k++)
                {
                    matrizC[i * colB + j] += matrizA[i * colA + k] * matrizB[k * colB + j];
                }
            }
        }
    }

    for (int i = 0; i < nThreads; i++)
    {
        if (pthread_join(*(tid + i), NULL))
        {
            fprintf(stderr, "ERRO--pthread_create\n");
            return 7;
        }
    }

    // Fim concorrente
    GET_TIME(fim);
    concorrente = fim - inicio;
    printf("%lf \n", concorrente);


    GET_TIME(inicio);
    descritorArquivo = fopen(argv[4], "wb");
    if (!descritorArquivo)
    {
        fprintf(stderr, "Erro de abertura do arquivo\n");
        return 3;
    }
    // escreve numero de linhas e de colunas
    ret = fwrite(&linA, sizeof(int), 1, descritorArquivo);
    ret = fwrite(&colB, sizeof(int), 1, descritorArquivo);
    // escreve os elementos da matriz
    ret = fwrite(matrizC, sizeof(float), tamC, descritorArquivo);
    if (ret < tamC)
    {
        fprintf(stderr, "Erro de escrita no  arquivo\n");
        return 9;
    }

    free(matrizA);
    free(matrizB);
    free(matrizC);
    
    GET_TIME(fim);
    final = fim - inicio;
    printf("%lf \n", final);
    
    return 0;
}