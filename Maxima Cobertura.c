//Gustavo Vidal Feitosa
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct{ // Struct dos pontos para receber os pontos do arquivo
    int indice;
    double latitude;
    double longitude;
    int densidade;
    double distanciaMaxima;
    double distanciaMinima;
    double distanciaMedia;
    int numeroQuadrasRaio;
    int densidadeTotalRaio;

} Ponto;

typedef struct{ // Struct da solução
    Ponto antenas[100];
    int qualidade;
    double cobertura;
} Solucao;

double distancia[2713][2713];
int antenas;
int raio_cobertura;
bool x[2713];

void lerArquivo(int numeroPontos, Ponto pontos[]){ // Função para a leitura do arquivo
    char linha[100];
    FILE *arq = fopen("NewYork.txt","rt");

    for(int i=0; i<5; i++)
        fgets(linha, 100, arq);

    char delimitador[] = " ";
    char *ptr;
    for (int i=0; i<numeroPontos; i++){
        fgets(linha, 100, arq);
        pontos[i].indice=i+1;
        ptr = strtok(linha, delimitador);
        sscanf(ptr, "%lf", &pontos[i].latitude);

        ptr = strtok(NULL, delimitador);
        sscanf(ptr, "%lf", &pontos[i].longitude);
    }

    fgets(linha, 100, arq);

    for (int i = 0; i < numeroPontos; i++){
        fgets(linha, 100, arq);
        pontos[i].densidade=atoi(linha);
    }
    fclose(arq);
}

int R = 6378100;

void infos(Ponto pontos[]){ // Função para calcular as distancias entre os pontos
    double lati, latj, longi, longj;

    for(int i=0; i<2713; i++){
        pontos[i].numeroQuadrasRaio = 0;
        pontos[i].densidadeTotalRaio = 0;

        for(int j=0; j<2713; j++){
            lati = pontos[i].latitude*M_PI/180;
            latj = pontos[j].latitude*M_PI/180;
            longi = pontos[i].longitude*M_PI/180;
            longj = pontos[j].longitude*M_PI/180;
            distancia[i][j] = R*acos(cos(lati)*cos(latj)*cos(longj - longi) + sin(lati)*sin(latj));

            //numero de quadras no raio e densidade total no raio de 400m
            if(distancia[i][j]<=400){
                pontos[i].numeroQuadrasRaio++;
                pontos[i].densidadeTotalRaio += pontos[j].densidade;
            }
        }
    }
}

void melhorSolucao(Ponto pontos[], Solucao *solucao) {
    int totalPessoas = 0;
    int pessoasMax, jMax;
    int qualidade = 0;
    int p = 100;
    int r = 400;
    solucao->qualidade = 0;
    bool x[2713];
    for(int i = 0; i < 2713; i++) {
        x[i] = false;
    }
    for(int i = 0; i < 2713; i++) {
        totalPessoas += pontos[i].densidade;
    }
    for(int i = 0; i < p; i++) {
        pessoasMax = 0;
        for(int j = 0; j < 2713; j++) {
            qualidade = 0;
            for(int k = 0; k < 2713; k++) {
                if(distancia[j][k] <= r && !x[k]) {
                    qualidade += pontos[k].densidade;
                }
            }
            if(qualidade > pessoasMax) {
                pessoasMax = qualidade;
                jMax = j;
            }
        }
        solucao->qualidade += pessoasMax;
        solucao->antenas[i] = pontos[jMax];
        for(int m = 0; m < 2713; m++){
            if(distancia[jMax][m] <= r && !x[m]) {
                x[m] = true;
            }
        }
    }
    solucao->cobertura = ((double)solucao->qualidade) / totalPessoas;
}

int main() {
    Ponto pontos[2713];
    Solucao solucao;
    lerArquivo(2713, pontos);
    infos(pontos);

    melhorSolucao(pontos, &solucao);

    // Imprime a melhor solução encontrada
    printf("antenas: {");
    for (int i = 0; i < 100; i++) {
        printf(" %d ", solucao.antenas[i].indice);
    }
    printf("}\n");
    printf("\nqualidade: %d\n", solucao.qualidade);
    printf("cobertura: %.2lf%%\n", solucao.cobertura * 100);

    return 0;
}