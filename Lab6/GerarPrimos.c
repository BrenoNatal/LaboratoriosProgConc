#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define TEXTO
#define ALEATORIO

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

int main(int argc, char *argv[])
{
   int *vetor, tot_primo = 0; // matriz que será gerada
   long long int tam;         // qtde de elementos no vetor
   FILE *descritorArquivo;    // descritor do arquivo de saida
   size_t ret;                // retorno da funcao de escrita no arquivo de saida

   // recebe os argumentos de entrada
   if (argc < 2)
   {
      fprintf(stderr, "Digite: %s <qtde de elementos> <arquivo saida>\n", argv[0]);
      return 1;
   }
   tam = atoi(argv[1]);

   // aloca memoria para a matriz
   vetor = (int *)malloc(sizeof(int) * tam);
   if (!vetor)
   {
      fprintf(stderr, "Erro de alocao da memoria do vetor\n");
      return 2;
   }

#ifdef ALEATORIO
   // randomiza a sequencia de numeros aleatorios
   srand(time(NULL));
   int num;
   for (long long int i = 0; i < tam; i++)
   {
      num = rand() % 9999999;
      *(vetor + i) = num;
      tot_primo += ehPrimo(num);
   }
#endif

#ifdef SEQUENCIAL
   for (long long int i = 0; i < tam; i++)
   {
      *(vetor + i) = i;
      tot_primo += ehPrimo(i);
   }
#endif

// imprimir na saida padrao a lista gerada
#ifdef TEXTO
   for (long int i = 0; i < tam; i++)
   {
      fprintf(stdout, "%d ", vetor[i]);
      fprintf(stdout, "\n");
   }
#endif

   // escreve a matriz no arquivo
   // abre o arquivo para escrita binaria
   descritorArquivo = fopen(argv[2], "wb");
   if (!descritorArquivo)
   {
      fprintf(stderr, "Erro de abertura do arquivo\n");
      return 3;
   }

   // escreve o tamanho do vetor
   ret = fwrite(&tam, sizeof(long long int), 1, descritorArquivo);

   // escreve o total de primos
   ret = fwrite(&tot_primo, sizeof(long long int), 1, descritorArquivo);

   printf("tot_primos: %d \n", tot_primo);

   // escreve os elementos do vetor
   ret = fwrite(vetor, sizeof(float), tam, descritorArquivo);
   if (ret < tam)
   {
      fprintf(stderr, "Erro de escrita no  arquivo\n");
      return 4;
   }

   // finaliza o uso das variaveis
   fclose(descritorArquivo);
   free(vetor);
   return 0;
}