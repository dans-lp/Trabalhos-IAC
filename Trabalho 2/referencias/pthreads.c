// include's do sistema
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

// meus include's

// minhas constantes
#define NUM_THREADS 4

// meus novos tipos de variáveis
struct thread_data {
    double *vetor1;
    double *vetor2;
    int qtd;
    int offset;
};

// meus protótipos
void *multiplica(void *p);

// variáveis globais

// minhas funções
int main(void) {
    double vetor1[] = {1,2,3,5,7,11,13,17,19,23,29,31};
    double vetor2[] = {37, 41, 43, 47, 53, 59, 61, 67, 71, 73, 79, 83};
    struct thread_data dados[NUM_THREADS];
    pthread_attr_t attr;
    pthread_t thread[NUM_THREADS];
    void *status;

    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    for(int i=0; i<NUM_THREADS; i++) {
        dados[i].vetor1 = vetor1;
        dados[i].vetor2 = vetor2;
        dados[i].qtd = ((sizeof vetor1) / (sizeof (double))) / NUM_THREADS;
        dados[i].offset = dados[i].qtd * i;
        int rc = pthread_create(&thread[i], &attr, multiplica, 
            (void *)&dados[i]);
        if(rc) {
            fprintf(stderr, "Erro ao criar a thread #%d\n", i);
            exit(1);
        }
    }
    pthread_attr_destroy(&attr);
    for(int i=0; i<NUM_THREADS; i++) {
        int rc = pthread_join(thread[i], &status);
        if(rc) {
            fprintf(stderr, "Não foi possível juntar com a thread #%d\n", i);
            exit(2);
        }
    }

    for(int i=0; i<sizeof vetor2 / sizeof(double); i++) {
        printf("%f ", vetor2[i]);
    }
    putchar('\n');

    return 0;
}

void *multiplica(void *p) {
    struct thread_data *pData;
    pData = (struct thread_data *) p;
    printf("Calculando a partir de %d\n", pData->offset);
    for(int i=pData->offset; i<pData->offset + pData->qtd; i++) {
        pData->vetor2[i] *= pData->vetor1[i];
    }
    pthread_exit(p);
}