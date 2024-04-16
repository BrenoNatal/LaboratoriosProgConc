#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "timer.h"
#include <pthread.h>

int main(int argc, char *argv[])
{

    double inicio, fim, inicializacao, final, sequencial;
    float *matrizA;                 // Matriz de entrada A
    float *matrizB;                 // Matriz de entrada B
    float *matrizSeq;               // Matriz para o resultado sequencial
    int linA, colA, linB, colB;     // Dimensões das matrizes de entrada
    long long int tamA, tamB, tamC; // Tamanho da matriz A
    FILE *descritorArquivo;         // descritor do arquivo de entrada
    size_t ret;                     // retorno da funcao de leitura no arquivo de entrada

    if (argc < 4)
    {
        printf("Digite: %s <arquivo da matriz A> <arquivo da matriz B> <arquivo de saida sequencial>\n", argv[0]);
        return 1;
    }

    // Inicializacao
    GET_TIME(inicio);
    
    // abre o arquivo para leitura binaria
    descritorArquivo = fopen(argv[1], "rb");
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
    descritorArquivo = fopen(argv[2], "rb");
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
        fprintf(stderr, "Erro de alocao da memoria da matriz B\n");
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
    matrizSeq = (float *)malloc(sizeof(float) * tamC);
    if (!matrizSeq)
    {
        fprintf(stderr, "Erro de alocao da memoria da matriz Sequencial\n");
        return 3;
    }

    GET_TIME(fim);
    inicializacao = fim - inicio;
    printf("%lf \n", inicializacao);

    GET_TIME(inicio);

    // Multiplicar matriz sequencial
    for (int i = 0; i < linA; i++)
    {
        for (int j = 0; j < colB; j++)
        {
            matrizSeq[i * colB + j] = 0;
            for (int k = 0; k < colA; k++)
            {
                matrizSeq[i * colB + j] += matrizA[i * colA + k] * matrizB[k * colB + j];
            }
        }
    }

    // Fim da parte sequencial
    GET_TIME(fim);
    sequencial = fim - inicio;
    printf("%lf \n", sequencial);


    GET_TIME(inicio);
    descritorArquivo = fopen(argv[3], "wb");
    if (!descritorArquivo)
    {
        fprintf(stderr, "Erro de abertura do arquivo\n");
        return 3;
    }


    // escreve numero de linhas e de colunas
    ret = fwrite(&linA, sizeof(int), 1, descritorArquivo);
    ret = fwrite(&colB, sizeof(int), 1, descritorArquivo);
    // escreve os elementos da matriz
    ret = fwrite(matrizSeq, sizeof(float), tamC, descritorArquivo);
    if (ret < tamC)
    {
        fprintf(stderr, "Erro de escrita no arquivo\n");
        return 4;
    }

    GET_TIME(fim);
    final = fim - inicio;
    printf("%lf \n", final);

    return 0;
}