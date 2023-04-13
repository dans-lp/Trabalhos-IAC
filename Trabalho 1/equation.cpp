#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <immintrin.h> 

#include "comum.h"
#include "timer.h"

/*
  Cálculo dos indexes intermerdiarios durante o armazenamento
  dos valores diagonais da matrix.
  
  O loop da função ArmazenaCoeficientesDiagonais opera
  de 4 em 4 valores para armazenalos diretamente num vetor AVX,
  portanto essa função foi criada para ser considerado dentro
  daquele loop valores de i entre 0..3 , 4..7, 8..11, ...*/
int IndexIntermediario(int i, int k)
{
  return (nIncognitas * (i + k)) + (i + k);
}

/*
  Seleciona os valores da diagonal da matriz apontada por hmA
  e através de um vetor AVX armazena estes no vetor de retorno*/
type_data *ArmazenaCoeficientesDiagonais(type_data *hmA) 
{
  type_data *coeficientesDiagonais = (type_data *)aligned_alloc(32, sizeof(type_data) * nIncognitas);
  __m256d tempAVX;

  for (int i = 0; i < nIncognitas - 1; i += 4) {
    tempAVX = _mm256_setr_pd(
      *(hmA + IndexIntermediario(i,0)),
      *(hmA + IndexIntermediario(i,1)),
      *(hmA + IndexIntermediario(i,2)),
      *(hmA + IndexIntermediario(i,3))
    );
    
    _mm256_store_pd((coeficientesDiagonais + i), tempAVX);
  }

  /*
    printf da array das diagonais
  */
  printf("\n\ndiagonais --->\n");
  for (int i = 0; i < nIncognitas - 1; i++){printf(" %f | ",coeficientesDiagonais[i]);}


  return coeficientesDiagonais;
}

/*
  modificar função para TODOS armazenar coeficientes a serem zerados*/
__m256d GeraMultiplicadorAVX(type_data *hmA, int contador)
{

  int valorIndex;
  type_data valorCoeficiente;
  type_data varK;

  valorIndex = (nIncognitas * contador) + contador;
  
  valorCoeficiente = *(hmA+valorIndex);
  
  /*
    //exibeMatriz(hmA, nIncognitas);
    //printf("\n\n hma: %p - nIn: %d - index: %d - cont: %d - Vcof %f\n\n",hmA, nIncognitas, valorIndex, contador, valorCoeficiente);
  */

  varK = *(hmA + valorIndex + nIncognitas) / valorCoeficiente;

  return _mm256_set1_pd(varK);
}


void processaVetores(double *hmA, double *hvB, int nIncognitas)
{
  int linhaIndex;

  // variáveis para guardar valores das equações
  //type_data *linhaCoeficientes = (type_data *)aligned_alloc(32, sizeof(type_data) * nIncognitas);
  type_data varK;
  type_data *coeficientesDiagonais;

  // vetores AVX
  __m256d coeficientes_mA;
  __m256d coeficientesOriginaisAVX_mA;
  __m256d multiplicadorAVX;
  __m256d avxTemp;
  __m256d  coeficientesResultantesAVX_mA;


  coeficientesDiagonais = ArmazenaCoeficientesDiagonais(hmA);

  //loop operando sobe o coeficiente à ser zerado de cada coluna
  for (int i = 0;  i < (nIncognitas - 1); i++){
    
    //as operações começam a partir da linha 1
    linhaIndex = (i+1) * nIncognitas; 

    multiplicadorAVX = GeraMultiplicadorAVX(hmA, i);
    
    //loop operando sobe os coeficientes da linha
    for (int j = 0; j < nIncognitas; j += 4) 
    {
      coeficientesOriginaisAVX_mA = _mm256_load_pd((hmA+linhaIndex) + j);
      /*
        ? 1.multiplicação AVX: multiplicador x coeficientes originais
        ? 2.subtração AVX: coeficientes origianais - coeficientes alterados
        ? 3.grava vetor alterado
        ? 4.alteração do vetor B igual passos acima 
        ? 5.grava vetor
      */
      avxTemp = _mm256_mul_pd(coeficientesOriginaisAVX_mA, multiplicadorAVX);
      coeficientesResultantesAVX_mA = _mm256_sub_pd(coeficientesOriginaisAVX_mA,avxTemp);

      //_mm256_store_pd((hmA + linhaIndex)+j, coeficientesResultantesAVX_mA);
      
    }
    

  }
  free(coeficientesDiagonais);
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