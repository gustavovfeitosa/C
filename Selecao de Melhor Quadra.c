//Gustavo Vidal Feitosa
#include <stdio.h>
#include <math.h>

typedef struct{
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

double distancia[2713][2713];

void lerArquivo(int numeroPontos, Ponto pontos[]){
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
    for(int i=0; i< 2713; i++){
         printf("Ponto indice: %d\n", pontos[i].indice);
        printf("latitude: %lf\n", pontos[i].latitude);
        printf("longitude: %lf\n", pontos[i].longitude);
        printf("densidade: %d\n", pontos[i].densidade);
        printf("distancia Maxima: %lf\n", pontos[i].distanciaMaxima);
        printf("distancia Minima: %lf\n", pontos[i].distanciaMinima);
        printf("distancia Media: %lf\n", pontos[i].distanciaMedia);
        printf("numero de Quadras no Raio: %d\n", pontos[i].numeroQuadrasRaio);
        printf("densidade Total no Raio: %d\n\n", pontos[i].densidadeTotalRaio);
    }
}

int R = 6378100;

void infos(Ponto pontos[]){
    double lati, latj, longi, longj, soma;
    double dmin=1000000000;
    double dmax=0;
    double dmedia=0;

    for(int i=0; i<2713; i++){
        pontos[i].numeroQuadrasRaio = 0;
        pontos[i].densidadeTotalRaio = 0;
        dmin = 1000000000;
        dmax = 0;
        dmedia = 0;

        for(int j=0; j<2713; j++){
            lati = pontos[i].latitude*M_PI/180;
            latj = pontos[j].latitude*M_PI/180;
            longi = pontos[i].longitude*M_PI/180;
            longj = pontos[j].longitude*M_PI/180;
            distancia[i][j] = R*acos(cos(lati)*cos(latj)*cos(longj - longi) + sin(lati)*sin(latj));

            soma += distancia[i][j];

            //Achar distancia minima
            if(dmin > distancia[i][j] && i != j){
                dmin = distancia[i][j];
            }

            //Achar distancia maxima
            if(dmax < distancia[i][j] && i!= j){
                dmax = distancia[i][j];
            }

            //numero de quadras no raio e densidade total no raio de 1km
            if(distancia[i][j]<=1000){
                pontos[i].numeroQuadrasRaio++;
                pontos[i].densidadeTotalRaio += pontos[j].densidade;
            }
        }
        pontos[i].distanciaMinima = dmin;
        pontos[i].distanciaMaxima = dmax;
        pontos[i].distanciaMedia = soma/2713;

        printf("Ponto indice: %d\n", pontos[i].indice);
        printf("latitude: %lf\n", pontos[i].latitude);
        printf("longitude: %lf\n", pontos[i].longitude);
        printf("densidade: %d\n", pontos[i].densidade);
        printf("distancia Maxima: %lf\n", pontos[i].distanciaMaxima);
        printf("distancia Minima: %lf\n", pontos[i].distanciaMinima);
        printf("distancia Media: %lf\n", pontos[i].distanciaMedia);
        printf("numero de Quadras no Raio: %d\n", pontos[i].numeroQuadrasRaio);
        printf("densidade Total no Raio: %d\n\n", pontos[i].densidadeTotalRaio);
    }
}

int restaurante(Ponto pontos[]){
    int melhorRestaurante, deni=0;
    //será a quadra que tiver maior densidade total no raio de 1km
    for(int i=0; i<2713; i++){
        if(pontos[i].densidadeTotalRaio > deni){
            deni = pontos[i].densidadeTotalRaio;
            melhorRestaurante = i+1;
        }
    }
    return melhorRestaurante;
}

int hotel(Ponto pontos[]){
    int melhorHotel, distmed;
    //será a quadra com menor distancia media entre os demais pontos, pois será o local mais proximo dos outros lugares na ilha
    for(int i=0; i<2713; i++){
        if(pontos[i].distanciaMedia < distmed){
            distmed = pontos[i].distanciaMedia;
            melhorHotel = i+1;
        }
    }
    return melhorHotel;
}

int main(){
    Ponto pontos[2713];
    lerArquivo(2713, &pontos);
    infos(pontos);

    printf("/n Melhor quadra para restaurante: %d", restaurante(pontos));
    printf("/n Melhor quadra para hotel: %d", hotel(pontos));

    return 0;
}
