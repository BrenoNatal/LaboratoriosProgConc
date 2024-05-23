#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <math.h>

// Variaveis globais
int *lista, tot_consumido = 0;
int *Buffer;
sem_t slotCheio, slotVazio, pegarItem; // semaforos para sincronizacao por condicao
sem_t mutexGeral;                      // semaforo UNICO para sincronizacao entre produtores e consumidores e para log

typedef struct
{
    int id;
    long long int tam;
    int bufferSize;

} t_args;

typedef struct
{
    int elem_checados, qtd_primos;
} result;

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

// funcao para inserir um elemento no buffer
void Insere(int item, int id, int bufferSize)
{
    static int in = 0;
    sem_wait(&slotVazio);  // aguarda slot vazio para inserir
    sem_wait(&mutexGeral); // exclusao mutua entre produtores (aqui geral para log)
    Buffer[in] = item;
    in = (in + 1) % bufferSize;
    sem_post(&mutexGeral);
    sem_post(&slotCheio); // sinaliza um slot cheio
}

// funcao para retirar  um elemento no buffer
int Retira(int id, int total, int buffer)
{
    int item;
    static int out = 0;
    sem_wait(&slotCheio);  // aguarda slot cheio para retirar
    sem_wait(&mutexGeral); // exclusao mutua entre consumidores (aqui geral para log)
    // Checa se todos os elementos da lista foram verificados
    item = Buffer[out];
    tot_consumido += 1;
    Buffer[out] = 0;
    out = (out + 1) % buffer;
    sem_post(&mutexGeral);
    sem_post(&slotVazio); // sinaliza um slot vazio
    return item;
}

//----------------------------------------------------------
// Produtor
void *produtor(void *param)
{
    t_args *args = (t_args *)param;

    for (int i = 0; i < args->tam; i++)
    {
        Insere(lista[i], 1, args->bufferSize);
    }
    pthread_exit(NULL);
}

//----------------------------------------------------------
// Consumidor
void *consumidor(void *param)
{
    t_args *args = (t_args *)param;
    int tam = args->tam;
    int item;
    int qtd_primos = 0;
    int elem_checados = 0;

    result *resultado;
    resultado = (result *)malloc(sizeof(result));

    while (1)
    {
        sem_wait(&pegarItem); // Agurda a retirada de item e evita entrar desnecessariamente na funcao Retira
        if (tot_consumido >= tam)
        {
            sem_post(&pegarItem);
            break;
        }
        item = Retira(args->id, tam, args->bufferSize);
        sem_post(&pegarItem);

        qtd_primos += ehPrimo(item);
        elem_checados += 1;
    }
    resultado->elem_checados = elem_checados;
    resultado->qtd_primos = qtd_primos;

    printf("Consumidor %d terminou \n", args->id);
    pthread_exit((void *)resultado);
}

int main(int argc, char *argv[])
{
    long long int tam;
    int tot_primos, consumidores, i, nThreads, buffer, mais_primos = 0, thread_vencedora = 0, qtd_primos = 0, elem_checados = 0;
    FILE *descritorArquivo; // descritor do arquivo de entrada
    size_t ret;             // retorno da funcao de leitura no arquivo de entrada
    result *resultado;      // retorna das threads

    if (argc < 4)
    {
        printf("Digite: %s <arquivo> <consumidores> <buffer>\n", argv[0]);
        return 1;
    }

    buffer = atoi(argv[3]);
    // Inicializar buffer
    Buffer = (int *)malloc(sizeof(int) * buffer);

    // Inicia os semaforos
    sem_init(&mutexGeral, 0, 1);     // binario
    sem_init(&slotCheio, 0, 0);      // contador
    sem_init(&slotVazio, 0, buffer); // contador
    sem_init(&pegarItem, 0, 1);      // binario

    // abre o arquivo para leitura binaria
    descritorArquivo = fopen(argv[1], "rb");
    if (!descritorArquivo)
    {
        fprintf(stderr, "Erro de abertura do arquivo\n");
        return 2;
    }

    // Qtd de consumidores e total de threads
    consumidores = atoi(argv[2]);
    nThreads = consumidores + 1;
    pthread_t tid[nThreads];

    // le a qtde de elementos
    ret = fread(&tam, sizeof(long long int), 1, descritorArquivo);

    // le a qtde de primos
    ret = fread(&tot_primos, sizeof(long long int), 1, descritorArquivo);

    printf("Total de primos: %d \n", tot_primos);

    lista = (int *)malloc(sizeof(int) * tam);

    // carrega a lista de elementos a serem checados
    ret = fread(lista, sizeof(int), tam, descritorArquivo);
    if (ret < tam)
    {
        fprintf(stderr, "Erro de leitura dos elementos da lista\n");
        return 3;
    }

    // Inicia a thread do produtor
    t_args *args = (t_args *)malloc(sizeof(t_args));
    if (args == NULL)
    {
        printf("--ERRO: malloc()\n");
        return 4;
    }

    // t_args para thread produtora
    args->id = 0;
    args->tam = tam;
    args->bufferSize = buffer;

    if (pthread_create(&tid[0], NULL, produtor, (void *)(args)))
    {
        printf("Erro na criacao do thread produtor\n");
        exit(1);
    }

    // Inicia os threads consumidores
    for (i = 0; i < consumidores; i++)
    {
        t_args *args = (t_args *)malloc(sizeof(t_args));
        if (args == NULL)
        {
            printf("--ERRO: malloc()\n");
            pthread_exit(NULL);
        }

        args->id = i + 1;
        args->tam = tam;
        args->bufferSize = buffer;

        if (pthread_create(&tid[1 + i], NULL, consumidor, (void *)(args)))
        {
            printf("Erro na criacao do thread produtor\n");
            exit(1);
        }
    }

    for (int i = 1; i < nThreads; i++)
    {
        if (pthread_join(*(tid + i), (void **)&resultado))
        {
            printf("--ERRO: pthread_join() \n");
            exit(-1);
        }
        printf("A thread %d checou %d elementos e %d eram primos \n", i, resultado->elem_checados, resultado->qtd_primos);
        qtd_primos += resultado->qtd_primos;
        elem_checados += resultado->elem_checados;
        if (mais_primos < resultado->qtd_primos)
        {
            mais_primos = resultado->qtd_primos;
            thread_vencedora = i;
        }
    }

    printf("As threads acharam %d primos em %d numeros checados do total de %d primos e %lld elementos. \n", qtd_primos, elem_checados, tot_primos, tam);
    printf("A thread vencedora foi a %d achando um total de %d primos. \n", thread_vencedora, mais_primos);

    free(lista);
    free(Buffer);
    free(args);
    free(resultado);
}