#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <immintrin.h> 
#include<pthread.h>

#include "comum.h"
#include "thread.h"
#include "timer.h"

// structs ==================================================================

struct thread_data{
  double *hmA;
  double *hvB;
  
  int coluna;
  type_data diagonal;

  int Id;
  int qtdLinhas;
  int offset;
} typedef Thread_data;


// protótipos de funções auxiliares =========================================

void *ProcessaLinhas(void *p);

void CalculaQtdLinhas(Thread_data *p, int coluna);


// ==========================================================================

void processaVetores(double *hmA, double *hvB, int nIncognitas)
{
  int coluna;
  type_data diagonal;

  //variaveis POXIS Threads
  Thread_data dados[nThreads];
  pthread_attr_t attr;
  pthread_t thread[nThreads];

  //variaveis para calculo das linhas
  int val = 0;
  int totalLinhas;
  int offset;

  //atributos threads
  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
  void *status;

  //loop operando por coluna
  for (coluna = 0; coluna < (nIncognitas - 1); coluna++){
    
    diagonal = *(hmA + (coluna * nIncognitas) + coluna);
    
    totalLinhas = nIncognitas - (coluna + 2);
    offset = coluna + 1;

    //inicialização das threads
    for (int i = 0; i < nThreads; i++){
      dados[i].Id = i;
      dados[i].hmA = hmA;
      dados[i].hvB = hvB;
      dados[i].coluna = coluna;
      dados[i].diagonal = diagonal;

      val = totalLinhas / (nThreads - i);
      dados[i].qtdLinhas = val;
      dados[i].offset = offset;
      //printf("\nwatch Thread values id-%d -> opera das linhas %d + %d",dados[i].Id, dados[i].offset, dados[i].qtdLinhas);
      
      offset += val;
      totalLinhas -= val;

      int rc = pthread_create(&thread[i], &attr, ProcessaLinhas, (void *)&dados[i]);
      if(rc) { 
        fprintf(stderr, "Erro ao criar a thread #%d\n", i);
        exit(1);
      }

    }
      
    for (int k = 0; k < nThreads; k++){
      int rc = pthread_join(thread[k], &status);
      if (rc){
        fprintf(stderr, "Não foi possível juntar com a thread #%d\n", k);
        exit(2);
      }
    }
    pthread_attr_destroy(&attr);
  }
  //printf("\n\n\n\n linha do erro -> \n\n");
  //exibeLinhaMatriz(hmA, 6, nIncognitas);
}


void *ProcessaLinhas(void *p)
{
  Thread_data *data;
  data = (Thread_data *)p;
  
  int linha;
  int linhaDiagonal;
  
  type_data coeficienteAbaixo; 
  int coeficienteAbaixoIndex;
  
  type_data valorLinha_vB;
  type_data varK;

  // variaveus MMX
  __m256d coeficientesOriginaisAVX_mA;
  __m256d multiplicadorAVX;
  __m256d avxTemp;
  __m256d coeficientesResultantesAVX_mA;
  __m256d coeficientesLinhaDiagonalAVX;
  
  linhaDiagonal = data->coluna * nIncognitas;
  


  //loop operando por linha
  for (int i = data->offset; i <= data->qtdLinhas; i++){
    
    //processamento do coeficiente multiplicador
    linha = i * nIncognitas;
    coeficienteAbaixoIndex = linha + data->coluna;
    coeficienteAbaixo = *(data->hmA + coeficienteAbaixoIndex);
    varK = -1 * (coeficienteAbaixo / data->diagonal);
    multiplicadorAVX = _mm256_set1_pd(varK);
    
    //loop operando sobe os coeficientes da linha
    for (int j = 0; j < nIncognitas; j += 4){
      coeficientesOriginaisAVX_mA = _mm256_load_pd(data->hmA + linha + j);
      coeficientesLinhaDiagonalAVX = _mm256_load_pd(data->hmA + linhaDiagonal + j); 
      
      avxTemp = _mm256_mul_pd(coeficientesLinhaDiagonalAVX, multiplicadorAVX);
      coeficientesResultantesAVX_mA = _mm256_add_pd(coeficientesOriginaisAVX_mA,avxTemp);
      _mm256_store_pd(data->hmA + linha + j, coeficientesResultantesAVX_mA);
    }

    valorLinha_vB = data->hvB[data->coluna];
    valorLinha_vB *= varK;
    data->hvB[i] += valorLinha_vB;

    if(data->Id == 1){ 
      printf("\nlinha %d - coeficiente abaixo %f\n",i, coeficienteAbaixo); 
      //exibeLinhaMatriz(data->hmA, i, nIncognitas ); 
    }
  }
    
  /*
   if(data->Id == 2){
   printf("\n\n -> coluna: %d\n",data->coluna);
   exibeMatriz(data->hmA, nIncognitas);
   printf("\n\n\n");
  }
  */
  
  pthread_exit(p);
}

//gera arquivos binarios com valores aleatórios em double para testes
/*
*/
void geraArquivos(char *nomeA, char *nomeB, int nIncognitas)
{
  FILE *arq;
  type_data valor;
  int maximo = 9;

  if ((arq = fopen(nomeA, "wb")) == NULL) {
    fprintf(stderr, "Não foi possível abrir o arquivo A para escrita\n");
    exit(2);
  }
  printf("\n--> Gera Matriz A\n");
  for (int i = 0; i < nIncognitas * nIncognitas; i++) {
    valor = (type_data)rand() / (type_data)(RAND_MAX / maximo);
    fwrite(&valor, sizeof(type_data), 1, arq);
  }
  fclose(arq);

  if ((arq = fopen(nomeB, "wb")) == NULL) {
    fprintf(stderr, "Não foi possível abrir o arquivo B para escrita\n");
    exit(2);
  }
  printf("\n--> Gera Vetor B\n");
  for (int i = 0; i < nIncognitas; i++) {
    valor = (type_data)rand() / (type_data)(RAND_MAX / maximo);
    fwrite(&valor, sizeof(type_data), 1, arq);
  }
  fclose(arq);
}