#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <immintrin.h> 

#include "comum.h"
#include "timer.h"


__m256d GeraMultiplicadorAVX(double *hmA, int contador)
{
  int valorIndex;
  type_data valorCoeficiente;
  type_data varK;

  valorIndex = (nIncognitas * contador) + contador;
  valorCoeficiente = *(hmA + valorIndex);
  varK = *(hmA + valorIndex + nIncognitas) / valorCoeficiente;

  return _mm256_set1_pd(varK);
}

/*
type_data *SalvaLinha(double *hmA)
{
  __m256d avxTemp;
  type_data *linhaCoeficientes = (type_data *)aligned_alloc(32, sizeof(type_data) * nIncognitas);

  for (int i = 0; i < nIncognitas; i += 4) {
    avxTemp = _mm256_load_pd(hmA + i);
    _mm256_store_pd((linhaCoeficientes + i), avxTemp);
  }

  return linhaCoeficientes;
}
*/

void processaVetores(double *hmA, double *hvB, int nIncognitas)
{
  // variáveis para guardar valores das equações
  type_data *coeficientesOriginais_vB = (type_data *)aligned_alloc(32, sizeof(type_data) * nIncognitas);
  //type_data *coeficientesOriginais_mA = (type_data *)aligned_alloc(32, sizeof(type_data) * nIncognitas);
  type_data varK;

  // vetores AVX
  __m256d coeficientes_mA;
  __m256d coeficientesOriginaisAVX_mA;
  __m256d multiplicadorAVX;
  __m256d avxTemp;
  __m256  coeficientesResultantesAVX_mA;
  //type_data *linhaCoeficientes = (type_data *)aligned_alloc(32, sizeof(type_data) * nIncognitas);

  
  for (int i = 0;  i < (nIncognitas - 1); i++){
    
    multiplicadorAVX = GeraMultiplicadorAVX(hmA, i);
    
    for (int j = 0; j < nIncognitas; j += 4)
    {
      // coeficientesOriginais_mA = SalvaLinha(hmA);
      
      coeficientesOriginaisAVX_mA = _mm256_load_pd(hmA + j);
      /*
        ? 1.multiplicação AVX: multiplicador x coeficientes originais
        ? 2.subtração AVX: coeficientes origianais - coeficientes alterados
        ? 3.grava vetor alterado
        ? 4.alteração do vetor B igual passos acima 
        ? 5.grava vetor
      */

      //_mm256_store_pd((linhaCoeficientes + i), avxTemp);
      
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