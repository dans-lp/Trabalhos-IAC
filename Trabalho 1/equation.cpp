#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <immintrin.h> 

#include "comum.h"
#include "timer.h"

// === Cabeçalho equation.cpp =================================
//=============================================================
void processaVetores(double *hmA, double *hvB, int nIncognitas);

/*Gera arquivos binarios contendo respectivamente uma matriz A
  e um vetor B com valores aleatórios em double */
void geraArquivos(char *nomeA, char *nomeB, int nIncognitas);
//=============================================================

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

  __m256d coeficientesOriginaisAVX_mA;
  __m256d multiplicadorAVX;
  __m256d avxTemp;
  __m256d coeficientesResultantesAVX_mA;
  __m256d coeficientesLinhaDiagonalAVX;

  //loop operando por coluna
  for (coluna = 0; coluna < (nIncognitas - 1); coluna++){

    diagonal = *(hmA + (coluna * nIncognitas) + coluna);
    printf("---->coluna: %d\n\n",coluna);
    
    //loop operando por linha
    for (int i = (coluna + 1); i < nIncognitas; i++){

      linha = i * nIncognitas;
      linhaDiagonal = coluna * nIncognitas;

      coeficienteAbaixoIndex = linha + coluna;
      coeficienteAbaixo = *(hmA + coeficienteAbaixoIndex);
      varK = -1 * (coeficienteAbaixo / diagonal);
      multiplicadorAVX = _mm256_set1_pd(varK);
      
      // ?
      //printf("\nlinha - i: %d | coeficiente abaixo: %f | varK: %f",i, coeficienteAbaixo, varK);

      //loop operando sobe os coeficientes da linha
      for (int j = 0; j < nIncognitas; j += 4){
        
        coeficientesOriginaisAVX_mA = _mm256_load_pd(hmA + linha + j);
        coeficientesLinhaDiagonalAVX = _mm256_load_pd(hmA + linhaDiagonal + j); 

        avxTemp = _mm256_mul_pd(coeficientesLinhaDiagonalAVX, multiplicadorAVX);
        coeficientesResultantesAVX_mA = _mm256_add_pd(coeficientesOriginaisAVX_mA,avxTemp);
        _mm256_store_pd(hmA + linha + j, coeficientesResultantesAVX_mA);
      }
      
      valorLinha_vB = hvB[i];
      valorLinha_vB *= varK;
      hvB[i] += valorLinha_vB;

    }
    //exibeMatriz(hmA, nIncognitas);
    exibeVetor(hvB, nIncognitas);
  }
}

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