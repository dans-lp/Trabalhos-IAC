/*
GPU
Versão Convencional:
- Transforma a matriz A e o vetor B de forma convencional

Versão 1: 
- Baseado na ideia original com tentativa de sincronização

Versão 2:
- Sem necessidade de sincronização porque utiliza um vetor auxiliar para armazenar os dados compartilhados entre as threads

Versão 3:
- Usa uma matriz auxiliar para armazenar o resultado

Versão 4:
- Distribuiu uniformemente os elementos da matriz pelos processadores
*/

#ifndef COMUM
#define COMUM

// macro de acesso à matriz
#define matriz(mA,linha,coluna,tamanho) (mA[(linha)*(tamanho)+(coluna)])
// calcula o menor de 2 números
#define menor(a,b)  (((a)<(b))? (a):(b))

// número de elementos máximo a serem impressos (linha e coluna)
#define MAX_PRINT   7

// erro máximo aceitável
#define ERRO_MAXIMO 0.01

// Constantes booleanas
#define FALSE 0
#define TRUE -1

// tipos de dados
typedef double type_data;

// Protótipos
void exibeMatriz(type_data *hmA, int nIncognitas);
void exibeVetor(type_data *hvB, int nIncognitas);
type_data *leMatriz(char *nome, int nIncognitas);
type_data *leVetor(char *nome, int nIncognitas);
type_data *calculaX(type_data *hmA, type_data *hvB, int nIncognitas);
void verificaResultado(type_data *hmA, type_data *hvX, type_data *hvB, int nIncognitas);
void exibeLinhaMatriz(type_data *hmA, int linha, int nIncognitas);
void processaVetores(double *hmA, double *hvB, int nIncognitas);
void parseArguments(int argc, char *argv[]);
void uso(char *nome);

// variáveis globais
extern int nIncognitas;
extern char *filenameMatrizA;
extern char *filenameVetorB;
extern char *filenameTempos;
extern int threadsPerBlock;
extern int blocksPerGrid;
extern int maxPrint;

/*
 Protótipos auxiliares de equation.cpp
 ! deletar depois !!! */

// gera arquivos binarios com os valores das respectivas matrizes declaradas pelo nome
void geraArquivos(char *nomeA, char *nomeB, int nIncognitas);

#endif