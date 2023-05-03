#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "comum.h"

void exibeMatriz(type_data *hmA, int nIncognitas) {
    fprintf(stderr, "============================================\n");
    for(int linha=0; linha<nIncognitas; linha++) {
        if(linha > maxPrint) {
            fprintf(stderr, "...\n");
            exibeLinhaMatriz(hmA, nIncognitas-1, nIncognitas);
            fputc('\n', stderr);
            break;
        }
        exibeLinhaMatriz(hmA, linha, nIncognitas);
        fputc('\n', stderr);
    }
    fprintf(stderr, "============================================\n");
}

void exibeLinhaMatriz(type_data *hmA, int linha, int nIncognitas) {
    for(int coluna=0; coluna<nIncognitas; coluna++) {
        if(coluna > maxPrint) {
            fprintf(stderr, "... %7.4f", matriz(hmA, linha, nIncognitas-1, nIncognitas));
            break;
        }
        fprintf(stderr, "%7.4f ", matriz(hmA, linha, coluna, nIncognitas));
    }
}

void exibeVetor(type_data *hvB, int nIncognitas) {
    fprintf(stderr, "============================================\n");
    for(int linha=0; linha<nIncognitas; linha++) {
        if(linha > maxPrint) {
            fprintf(stderr, "... %7.4f", hvB[nIncognitas -1]);
            break;
        }
        fprintf(stderr, "%7.4f ", hvB[linha]);
    }
    fputc('\n', stderr);
    fprintf(stderr, "============================================\n");
}

type_data *leMatriz(char *nome, int nIncognitas) {
    type_data *hmA;
    FILE *arq;
    int qtd;

    if(!(hmA = (type_data *)aligned_alloc(32, sizeof(type_data) * nIncognitas * nIncognitas))) {
        fprintf(stderr, "Não foi possível alocar memória para a matriz A[%d, %d]\n", nIncognitas, nIncognitas);
        exit(1);
    }
    if(!(arq = fopen(nome, "rb"))) {
        fprintf(stderr, "Não foi possivel abrir o arquivo %s para leitura\n", nome);
        exit(6);
    }
    if((qtd = fread(hmA, sizeof(type_data), nIncognitas*nIncognitas, arq)) != nIncognitas*nIncognitas) {
        fprintf(stderr, "Tamanho de arquivo incompatível (%d != %d)\n", qtd, nIncognitas*nIncognitas);
        exit(7);
    }
    fclose(arq);
    return hmA;
}

type_data *leVetor(char *nome, int nIncognitas) {
    type_data *hvB;
    FILE *arq;
    int qtd;

    if(!(hvB = (type_data *)aligned_alloc(32, sizeof(type_data) * nIncognitas))) {
        fprintf(stderr, "Não foi possível alocar memória para o vetor B[%d]\n", nIncognitas);
        exit(2);
    }
    if(!(arq = fopen(nome, "rb"))) {
        fprintf(stderr, "Não foi possivel abrir o arquivo %s para leitura\n", nome);
        exit(4);
    }
    if((qtd = fread(hvB, sizeof(type_data), nIncognitas, arq)) != nIncognitas) {
        fprintf(stderr, "Tamanho de arquivo incompatível (%d != %d)\n", qtd, nIncognitas);
        exit(5);
    }
    fclose(arq);
    return hvB;
}

type_data *calculaX(type_data *hmA, type_data *hvB, int nIncognitas) {
    type_data *hvX;

    // cria espaço em memória para o vetor X
    if(!(hvX = (type_data *)aligned_alloc(32, sizeof(type_data) * nIncognitas))) {
        fprintf(stderr, "Não foi possível alocar memória para o vetor X[%d]\n", nIncognitas);
        exit(2);
    }

    // Calcula o vetor X
    for(int diagonal=nIncognitas-1; diagonal>=0; diagonal--) {
        type_data somatorio = 0;
        for(int coluna=nIncognitas-1; coluna>diagonal; coluna--) {
            somatorio += hvX[coluna] * matriz(hmA, diagonal, coluna, nIncognitas);
            if(isnan(somatorio)) {
                fprintf(stderr, "Somatório virou NaN em %d,%d (%f %f)\n", diagonal, coluna, hvX[coluna], matriz(hmA, diagonal, coluna, nIncognitas));
                exit(-1);
            }
        }
        hvX[diagonal] = (hvB[diagonal] - somatorio) / matriz(hmA, diagonal, diagonal, nIncognitas);
    }

    // Exibe o resultado
    fprintf(stderr, "X =\n");
    exibeVetor(hvX, nIncognitas);

    return hvX;
}

void verificaResultado(type_data *hmA, type_data *hvX, type_data *hvB, int nIncognitas) {
    printf("Verificando resultado... ");
    for(int linha=0; linha<nIncognitas; linha++) {
        type_data somatorio=0;
        for(int coluna=0; coluna<nIncognitas; coluna++) {
            somatorio += matriz(hmA, linha, coluna, nIncognitas) * hvX[coluna];
        }
        if(abs(somatorio - hvB[linha]) > somatorio * 0.01 || abs(somatorio - hvB[linha]) > hvB[linha] * 0.1 || isnan(somatorio)) {
            fprintf(stderr, "Resultado errado na linha %d (%f != %f)\n", linha, somatorio, hvB[linha]);
            exit(8);
        }
    }
    printf("Ok\n");
}