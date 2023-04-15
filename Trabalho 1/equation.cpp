#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <immintrin.h> 

#include "comum.h"
#include "timer.h"


// === Cabeçalho equation.cpp =================================
//=============================================================
/*Cálculo dos indexes intermerdiarios durante o armazenamento
  dos valores diagonais da matrix.
  
  O loop da função ArmazenaCoeficientesDiagonais opera
  de 4 em 4 valores para armazenalos diretamente num vetor AVX,
  portanto essa função foi criada para ser considerado dentro
  daquele loop valores de i entre 0..3 , 4..7, 8..11, ...*/
int IndexIntermediario(int i, int k);

/*Seleciona os valores da diagonal da matriz apontada por hmA
  e através de um vetor AVX armazena estes no vetor de retorno*/
type_data *ArmazenaCoeficientesDiagonais(type_data *hmA);

void processaVetores(double *hmA, double *hvB, int nIncognitas);

/*Gera arquivos binarios contendo respectivamente uma matriz A
  e um vetor B com valores aleatórios em double */
void geraArquivos(char *nomeA, char *nomeB, int nIncognitas);
//=============================================================

void processaVetores(double *hmA, double *hvB, int nIncognitas)
{
  type_data diagonal;
  type_data coeficienteInicial; 
  type_data varK;
  
  int coluna;
  int linha;
  int linhaInicio;
  int inicioIndex;

  // ? __m256d coeficientes_mA;
  __m256d coeficientesOriginaisAVX_mA;
  __m256d multiplicadorAVX;
  __m256d avxTemp;
  __m256d coeficientesResultantesAVX_mA;

  //loop operando por coluna
  for (coluna = 0; coluna < (nIncognitas - 1); coluna++){

    diagonal = *(hmA + (coluna * nIncognitas) + coluna);
    linha = (coluna + 1) * nIncognitas; //as operações começam a partir da linha 1
    linhaInicio = linha + coluna;
    //coluna  = 

    //loop operando por linha, começando abaixo da diagonal
    for (int i = 0; i < (nIncognitas - 1); i++){
      
      inicioIndex = linhaInicio + (nIncognitas * i);
      coeficienteInicial = *(hmA + inicioIndex);
      varK = coeficienteInicial / diagonal;
      multiplicadorAVX = _mm256_set1_pd(varK);

      // ? teste loop 2
      printf("\n\n === teste valor k ===\n");
      printf("val: %f | diagonal: %f",coeficienteInicial, diagonal);

      //loop operando sobe os coeficientes da linha
      int coeficienteIndex = 0;
      for (; coeficienteIndex < nIncognitas; coeficienteIndex += 4){
        coeficientesOriginaisAVX_mA = _mm256_load_pd(hmA + linha + coeficienteIndex);
        /*
          ? 1.multiplicação AVX: multiplicador x coeficientes originais
          ? 2.subtração AVX: coeficientes origianais - coeficientes alterados
          ? 3.grava vetor alterado
          ? 4.alteração do vetor B igual passos acima 
          ? 5.grava vetor
        */
        avxTemp = _mm256_mul_pd(coeficientesOriginaisAVX_mA, multiplicadorAVX);
        coeficientesResultantesAVX_mA = _mm256_sub_pd(coeficientesOriginaisAVX_mA,avxTemp);
       // _mm256_store_pd(hmA + linha + coeficienteIndex, coeficientesResultantesAVX_mA);
      }
        //? teste loop 3
        /*
        */
        printf("\n\n ===== teste valores originais ====\n");
        for (int i = 0; i < 4; i++)
        {
          printf(" %f |",*(hmA + linha + coeficienteIndex));
        }
    }
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