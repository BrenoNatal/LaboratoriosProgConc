//Soma os elementos de um vetor de float
#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>

#define TESTE

//escopo global (par ser compartilhado com as threads)
float *vetor; //vetor de entrada

//define o tipo de retorno das threads
typedef struct {
    long int tamBloco;
    int idThread;
} t_Args;

void *Somar(void* arg) {
    t_Args *args = (t_Args *) arg;

    float *sum_temp;
    sum_temp = (float*) malloc(sizeof(float));
    *sum_temp = 0;

    if (args->tamBloco){
        long int inicio = args->idThread * args->tamBloco;
        long int fim = inicio + args->tamBloco;
        *sum_temp = vetor[inicio];
        for (long int i = inicio + 1; i < fim; i++) {
            //printf("\nSomando %f com %f na thread %d", vetor[i], *sum_temp, args->idThread);
            *sum_temp += vetor[i];
            //printf(" A soma da %f", *sum_temp);
        }
    }

    printf("\nRetorno da thread %d: %f\n", args->idThread, *sum_temp);
    pthread_exit((void *) sum_temp);

}


int main(int argc, char *argv[]){
    float somaGeral; //valores encontrados pela solucao concorrente
    long int dim; //dimensao do vetor de entrada
    long int tamBloco; //tamanho do bloco de cada thread 
    int nthreads; //numero de threads que serao criadas

    pthread_t *tid; //vetor de identificadores das threads no sistema
    float *retorno; //valor de retorno das threads

    #ifdef TESTE
    float somaCorreta;
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
    scanf("%f", &somaCorreta);
    for(long int i=0; i<dim; i++)
      printf("%f ", vetor[i]); 
    #endif 

    somaGeral = 0;

    //cria as threads
    tid = (pthread_t *) malloc(sizeof(pthread_t) * nthreads);
    if(tid==NULL) {
        fprintf(stderr, "ERRO--malloc\n");
        return 3;
    }

    tamBloco = dim / nthreads;

    if(!tamBloco) printf("\nA quantidade de threads eh maior que a quantidade de elementos, a execucao sera sequencial!\n");

    for (int i = 0; i < nthreads; i++){

        t_Args *args = (t_Args*) malloc(sizeof(t_Args));
        if(args == NULL) {
            printf("--ERRO: malloc()\n");
            pthread_exit(NULL);
        }
        args->idThread = i;
        args->tamBloco = tamBloco;
        if (pthread_create(tid+i, NULL, Somar, (void*) args)) { 
            fprintf(stderr, "ERRO--pthread_create\n");
            return 5;
        }
    }

    if(dim%nthreads) {
        puts("\nMain com tarefa");
        for(long int i=dim-(dim%nthreads); i<dim; i++) {
            somaGeral += vetor[i];
        }
   } 

    for (int i = 0; i < nthreads; i++) {
        if(pthread_join(*(tid+i), (void**) &retorno)){
            fprintf(stderr, "ERRO--pthread_create\n");
            return 6;
        }
        //printf("\nSomando %f ao total de %f na thread %d ", *retorno, somaGeral, i);
        somaGeral += *retorno;
        //printf("O resultado é %f", somaGeral);
    }

    printf("\n\nSoma de todos os elemetos:%f", somaGeral);

    #ifdef TESTE
    printf("\n\nSoma de todos os elementos correto: %f \n", somaCorreta);
    #endif

    //libera as areas de memoria alocadas
    free(vetor);
    free(tid);
    free(retorno);

    return 0;
}
