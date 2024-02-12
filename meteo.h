/* 
 * File:   meteo.h
 * Author: Diego Pérez Álvarez
 *
 * Created on 11 de diciembre de 2023, 15:51
 */

#ifndef BIBLIOTECA_H
#define BIBLIOTECA_H

#ifdef __cplusplus
extern "C" {
#endif
    
typedef struct{
    int dia;
    int mes;
    int ano;
}Fecha;     //Estrutura para a data

typedef struct{
    Fecha date;
    float max_temp;
    float min_temp;
    float precipitation;
    float wind;
    float rel_hum;
    float solar;
}WeatherData;       //Estrutura para os datos recollidos nunha estación e nunha data concreta

typedef struct{
    char nome[256];         
    float lonxitude;
    float latitude;
    float elevacion;
}Station;       //Estrutura para os datos de localización dunha estación

typedef struct{
    WeatherData *vector;
    Station estacion;
}WeatherStation;        //Estrutura que une os datos de posicion dunha estación e todos os datos meteorolóxicos recollidos nela

typedef struct{
    Station estacion;
    float valor;
    Fecha fecha;
}Extreme;       //Estrutura que agrupa todos os datos relativos a un extremo de temperatura para facilitar o seu retorno: estación, valor e data


int Bisiesto(Fecha *fecha);
int DataValue(Fecha fecha);
char *Concatenate(char first[], char last[]);
WeatherStation *CrearVector(FILE *nameFile, int *vectorSize);
WeatherStation *GetStationData(char name[], int readFiles, WeatherStation *vector);
Station AddStation(FILE* stationFile, char stationName[], int *erro);
WeatherData *AddDataVector(FILE *stationFile, int *erro);
Extreme *FindExtreme(WeatherStation *vector, int vectorSize, Fecha fecha1, Fecha fecha2);
float *FindAverage(WeatherStation *vector, int vectorSize, Fecha fecha);
void SafeExit(WeatherStation *vector, int vectorSize, int code, char message[]);

#ifdef __cplusplus
}
#endif

#endif /* BIBLIOTECA_H */