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

    /*
    //?
    printf("\n\n ---> iteração %d",coluna);
    */
    //loop operando por linha, começando abaixo da diagonal
    for (int i = 0; i < (nIncognitas - (coluna + 1)); i++){
      
      inicioIndex = linhaInicio + (nIncognitas * i);
      coeficienteInicial = *(hmA + inicioIndex);
      varK = coeficienteInicial / diagonal;
      multiplicadorAVX = _mm256_set1_pd(varK);

      /*
      // ? teste loop 2
      printf("\n\n === teste valor k ===\n");
      printf("val: %f | diagonal: %f",coeficienteInicial, diagonal);
      */

      //loop operando sobe os coeficientes da linha
      for (int coeficienteIndex = 0; coeficienteIndex < nIncognitas; coeficienteIndex += 4){
        
        coeficientesOriginaisAVX_mA = _mm256_load_pd(hmA + inicioIndex + coeficienteIndex);
        avxTemp = _mm256_mul_pd(coeficientesOriginaisAVX_mA, multiplicadorAVX);
        coeficientesResultantesAVX_mA = _mm256_sub_pd(coeficientesOriginaisAVX_mA,avxTemp);
       //_mm256_store_pd(hmA + linha + coeficienteIndex, coeficientesResultantesAVX_mA);
      
        //? teste loop 3
        /*
        if(coeficienteIndex == 0){
          printf("\n\n ===== teste valores originais ====\n");
          for (int i = 0; i < 4; i++){
            printf(" %f |",*(hmA + inicioIndex + i));
          }
        }
        */
      }
      // ? alteração do valor do vetir B
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