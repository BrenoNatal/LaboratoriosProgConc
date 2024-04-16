/*
    Programa para gerar o vetor de entrada e o resultado correto de teste.
*/


#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX 100

int main(int argc, char *argv[]){
    long int n;
    float elem, soma;

    if (argc < 2)
    {
        printf("Use: %s <numero de elementos (maior que 0)> \n", argv[0]);
        return 1;
    }
    n = atol(argv[1]);
    if(n<1) {
       printf("ERRO: o numero de elementos deve ser maior que 0 \n");
       return 2;
    }	

    //semente para gerar numeros randomicos
    srand((unsigned)time(NULL));

    //imprime a qtde de elementos do vetor (inteiro)
    printf("%ld\n", n);

    elem = (((float)rand() / RAND_MAX) * (2 * MAX)) - MAX;
    soma = elem;
    printf("%f ", elem);
    for (int i = 0; i < n - 1; i++){
        elem = (((float)rand() / RAND_MAX) * (2 * MAX)) - MAX;
        soma += elem;
        printf("%f ", elem);
    }

    //imprime a soma
    printf("%f ", soma);

    return 0;

}