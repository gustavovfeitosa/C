//Gustavo Vidal Feitosa
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct {
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

typedef struct {
    Ponto *antenas;
    int qualidade;
    double cobertura;
} Solucao;

double **distancia;

void lerArquivo(int *numeroPontos, Ponto **pontos, int *raio) { // Função para ler arquivo
    *numeroPontos = 0;
    char linha[100];
    FILE *arq = fopen("NewYork.txt", "rt");

    for (int i = 0; i < 4; i++)
        fgets(linha, 100, arq);
    for (int i = 12; linha[i] >= '0' && linha[i] <= '9'; i++) {
        *numeroPontos *= 10;
        *numeroPontos += linha[i] - '0';
    }

    fgets(linha, 100, arq);
    for (int i = 9; linha[i] >= '0' && linha[i] <= '9'; i++) {
        *raio *= 10;
        *raio += linha[i] - '0';
    }

    char delimitador[] = " ";
    char *ptr;

    *pontos = (Ponto *)malloc((*numeroPontos) * sizeof(Ponto));

    for (int i = 0; i < *numeroPontos; i++) {
        fgets(linha, 100, arq);
        (*pontos)[i].indice = i + 1;
        ptr = strtok(linha, delimitador);
        sscanf(ptr, "%lf", &(*pontos)[i].latitude);

        ptr = strtok(NULL, delimitador);
        sscanf(ptr, "%lf", &(*pontos)[i].longitude);
    }

    fgets(linha, 100, arq);

    for (int i = 0; i < *numeroPontos; i++) {
        fgets(linha, 100, arq);
        (*pontos)[i].densidade = atoi(linha);
    }
    fclose(arq);
}

int R = 6378100;

void calcularDistancias(Ponto pontos[], int n, int *raio) { // Função para calcular as distancias entre os pontos
    double lati, latj, longi, longj;
    distancia = (double **)malloc(n * sizeof(double *));
    for (int i = 0; i < n; i++) {
        distancia[i] = (double *)malloc(n * sizeof(double));
    }

    for (int i = 0; i < n; i++) {
        pontos[i].numeroQuadrasRaio = 0;
        pontos[i].densidadeTotalRaio = 0;

        for (int j = 0; j < n; j++) {
            lati = pontos[i].latitude * M_PI / 180;
            latj = pontos[j].latitude * M_PI / 180;
            longi = pontos[i].longitude * M_PI / 180;
            longj = pontos[j].longitude * M_PI / 180;
            distancia[i][j] = R * acos(cos(lati) * cos(latj) * cos(longj - longi) + sin(lati) * sin(latj));
        }
    }
}

void acharSolucao(Ponto pontos[], Solucao *solucao, int r, int n) { // Função para achar a solucao
    int totalPessoas = 0;
    int nPessoas = 0;
    solucao->qualidade = 0;
    bool x[n];

    for (int i = 0; i < n; i++) { // Inicializa todos os pontos como sem antena
        x[i] = false;
        totalPessoas += pontos[i].densidade;
    }

    while (solucao->cobertura < 1.0) { // Looping continua até que a cobertura seja 100%
        int maxPontos = 0;
        int jMax = -1;

        for (int i = 0; i < n; i++) { // Para cada antena faremos a análise
            if (!x[i]) {
                int pontosNaoCobertos = 0;
                for (int j = 0; j < n; j++) {
                    if (distancia[i][j] <= r && !x[j]) { // Conta quantos pontos não cobertos estão em volta (no raio r) da antena analisada
                        pontosNaoCobertos++;
                    }
                }
                if (pontosNaoCobertos > maxPontos) { // Escolhe o ponto com mais pontos não cobertos ao redor
                    maxPontos = pontosNaoCobertos;
                    jMax = i;
                }
            }
        }

        if (jMax == -1) {
            break;
        }

        solucao->antenas[solucao->qualidade].indice = pontos[jMax].indice + 1; // Passa o indice do ponto escolhido para a solucao
        solucao->qualidade++; // Contabiliza quantas antenas foram usadas
        nPessoas += pontos[jMax].densidade; // Atualiza a quantidade de pessoas cobertas
        for (int i = 0; i < n; i++) { // Atualiza a cobertura dos pontos ao redor
            if (distancia[jMax][i] <= r && !x[i]) {
                nPessoas += pontos[i].densidade;
            }
        }
        x[jMax] = true; // Atualiza a cobertura do ponto escolhido

        for (int i = 0; i < n; i++) { // Atualiza a cobertura dos pontos ao redor
            if (distancia[jMax][i] <= r && !x[i]) {
                x[i] = true;
            }
        }

        solucao->cobertura = (double)nPessoas / totalPessoas; // Atualiza o valor da cobertura
    }
}

void imprimirRespostas(Solucao solucao) { // Imprime as respostas
    printf("Antenas: {");
    for (int i = 0; i < solucao.qualidade; i++) {
        printf(" %d ", solucao.antenas[i].indice);
    }
    printf("}\n");
    printf("Qualidade: %d\n", solucao.qualidade);
}

int main() { // Função principal
    int numeroPontos = 0;
    int raio = 0;
    Ponto *pontos = NULL;
    Solucao solucao;

    lerArquivo(&numeroPontos, &pontos, &raio);

    solucao.antenas = (Ponto *)malloc(numeroPontos * sizeof(Ponto)); // Alocação de memória da solução

    distancia = (double **)malloc(numeroPontos * sizeof(double *)); // Alocação de memória da matriz de distâncias
    for (int i = 0; i < numeroPontos; i++) {
        distancia[i] = (double *)malloc(numeroPontos * sizeof(double));
    }

    calcularDistancias(pontos, numeroPontos, &raio);

    acharSolucao(pontos, &solucao, raio, numeroPontos);

    imprimirRespostas(solucao);

    for (int i = 0; i < numeroPontos; i++) { // Libera memória alocada
        free(distancia[i]);
    }
    free(distancia);
    free(pontos);

    return 0;
}