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
  int diagonal;

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
  int linha;
  
  type_data diagonal;
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

  //variaveis POXIS Threads
  Thread_data dados[nThreads];
  pthread_attr_t attr;
  pthread_t thread[nThreads];
  void *status;

  //atributos threads
  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
  //int rc;

  //loop operando por coluna
  for (coluna = 0; coluna < (nIncognitas - 1); coluna++){
    
    diagonal = *(hmA + (coluna * nIncognitas) + coluna);
    
    CalculaQtdLinhas(dados, coluna);

    //inicialização das threads
    for (int i = 0; i < nThreads; i++)
    {
      dados[i].hmA = hmA;
      dados[i].hvB = hvB;
      dados[i].coluna = coluna;
      dados[i].diagonal = diagonal;
      dados[i].Id = i;

      // int rc = pthread_create();
    }

    // transformar esse pedaço na função ProcessaLinhas
    
      //loop operando por linha
      for (int i = (coluna + 1); i < nIncognitas; i++){

        linha = i * nIncognitas;
        linhaDiagonal = coluna * nIncognitas;

        coeficienteAbaixoIndex = linha + coluna;
        coeficienteAbaixo = *(hmA + coeficienteAbaixoIndex);
        varK = -1 * (coeficienteAbaixo / diagonal);
        multiplicadorAVX = _mm256_set1_pd(varK);

        //loop operando sobe os coeficientes da linha
        for (int j = 0; j < nIncognitas; j += 4){

          coeficientesOriginaisAVX_mA = _mm256_load_pd(hmA + linha + j);
          coeficientesLinhaDiagonalAVX = _mm256_load_pd(hmA + linhaDiagonal + j); 

          avxTemp = _mm256_mul_pd(coeficientesLinhaDiagonalAVX, multiplicadorAVX);
          coeficientesResultantesAVX_mA = _mm256_add_pd(coeficientesOriginaisAVX_mA,avxTemp);
          _mm256_store_pd(hmA + linha + j, coeficientesResultantesAVX_mA);
        }

        valorLinha_vB = hvB[coluna];
        valorLinha_vB *= varK;
        hvB[i] += valorLinha_vB;
      }
    //
  }
}

void CalculaQtdLinhas(Thread_data *p, int coluna)
{
  int val = 0;
  int totalLinhas = nIncognitas - (coluna + 1);
  int offset = 0;

  for (int i = 0; i < nThreads; i++, totalLinhas -= val){
    val = totalLinhas / (nThreads - i);
    p[i].qtdLinhas = val;
    p[i].offset = offset;
    offset += val;
  }
}

/*
  0 --> linha 1
  1 --> linha 7
  2 --> 
*/

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