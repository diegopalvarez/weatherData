/* 
 * File:   main.c
 * Author: Diego Pérez Álvarez
 * Created on 11 de diciembre de 2023, 15:54
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "meteo.h"

char *Concatenate(char first[], char last[]){
    /*
    Precondición: Introducir dúas cadeas de caracteres.
    Concatenate(first, last);
    Poscondición: Devolver unha única cadea de caracteres de lonxitude igual á suma de lonxitudes das dúas cadeas introducidas, menos o caracter '\0' intermedio
    */
    char *fullPath;
    fullPath = (char *)malloc(sizeof(char)*strlen(first)+strlen(last)-1);
    int i, n;
    for (i = 0; i<strlen(first); i++){
        fullPath[i]=first[i];
    }
    for (n = 0; n<strlen(last); n++){
        fullPath[i+n]=last[n];
    }
    fullPath[i+n]='\0';
    return fullPath;
}

int Bisiesto(Fecha *fecha){
    /*
    Precondición: Pasar un punteiro de unha estrutura Fecha cos seus campos inicializados.
    Bisiesto(fecha);
    Poscondición: Retornar 1 se o mes é Febreiro e o ano é Bisiesto. Se non, retornar 0.
    */
    if ((fecha->mes == 2) && (((fecha->ano)%4==0 && (fecha->ano)%100 != 0) || ((fecha->ano)%400) == 0)){
        return 1;
    }
    else
        return 0;
}

int meses[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};   //Vector co número de días por mes (nun ano non bisiesto)

int DataValue(Fecha fecha){
    /*
    Precondición: Introducir unha estrutura Fecha cos seus campos inicializados.
    DataValue(fecha);
    Poscondicón: Retornar 0 se a data é válida e está dentro do rango de estudo (2000-2010) ou 1 se non o está.
    */
    int max_mes;
    max_mes = meses[fecha.mes-1] + Bisiesto(&fecha);
    if (fecha.ano>2010 || fecha.ano<2000 || fecha.mes<1 || fecha.mes>12 || fecha.dia<1 ||fecha.dia>max_mes)
        return 1;
    else
        return 0;
}

void SafeExit(WeatherStation *vector, int vectorSize, int code, char message[]){
    /*
    Precondición: Pasar un punteiro a un vector de estruturas WeatherStation con vectorSize elementos e un código de erro, así como unha mensaxe.
    SafeExit(vector, vectorSize, code);
    Poscondición: Un procedemento non devolve ningún valor, pero liberará a memoria de todos os vectores de WeatherData dentro do vector, e rematará por liberar a memoria do propio vector.
                  Remata o programa.
    */
    int i;
    if (vector != NULL){
        for (i = 0; i<vectorSize; i++){     //Liberamos todos os vectores de WeatherData que xa foron creados
            free(vector[i].vector);
        }
        free(vector);       //Liberamos a memoria do vector de WeatherStations
    }
    switch (code){      //Imprimimos o mensaxe de erro apropiado
        case 0: break;
        case 1:
            printf("Error opening the file %s (Error %d)", message, code);
            break;
        default:
            printf("%s (Error %d)", message, code);
            break;     
    }
    exit(code);
}

WeatherStation *CrearVector(FILE *nameFile, int *vectorSize){
    /*
    Precondición: Pasar un punteiro ao arquivo cos nomes dos arquivos das estacións en liñas diferentes e unha variable de saída para o número de elementos do vector 
    CrearVector(nameFile, &vectorSize);
    Poscondición: Devolver o punteiro a un vector dinámico de estruturas WeatherStation xa cheas con cadanseu dato da estación e vector dos datos meteorolóxicos recollidos.
                  Tamén se modifica o valor almacenado na variable vectorSize, que pasa a ser o número de elementos do vector de WeatherStation
    */
    char name[256];
    int size;
    size = 0;
    while (fgets(name, 256, nameFile) != NULL) {        //Atopar o número de arquivos a analizar
        size++;
    }
    *vectorSize = size;     //Actualizamos o valor do número de elementos do vector para utilizalo logo ao liberar a memoria
    fseek(nameFile, 0, SEEK_SET);       //Retornamos o cursor ao comezo do arquivo
    WeatherStation *vector;
    vector = (WeatherStation *)malloc(sizeof(WeatherStation)*size);     //Crear o vector co número necesario de elementos
    
    int i = 0;
    int readFiles = 0;
    while (fscanf(nameFile, "%s\n", name)!=EOF){        
        vector[i] = *GetStationData(name, readFiles, vector);        
        i++;
        readFiles++;
    }
    return vector;
}

WeatherStation *GetStationData(char name[], int readFiles, WeatherStation *vector){
    /*
    Precondición: Pasar un nome de arquivo, un punteiro a un vector de WeatherStations e o número de vectores WeatherData creados ata o momento.
    GetStationData(name, readFiles, vector);
    Poscondición: Retornar unha estrutura WeatherStation cos datos recollidos no arquivo CSV correspondente.
    */
    FILE *stationFile;
    char path[] = "45592_2021-07-01-13-45-07/";
    char *fullPath;
    fullPath = Concatenate(path, name); //Unir o path co nome de arquivo pasado como argumento
    
    if ((stationFile = fopen(fullPath, "r")) == NULL) { //Abrir o arquivo. Se temos un erro, liberamos todos os vectores xa creados e paramos o programa
        SafeExit(vector, readFiles, 1, name);
    }
    
    char stationName[256];
    sscanf(name, "%[^.]", stationName);
    
    WeatherStation *meteoData;
    meteoData = (WeatherStation *)malloc(sizeof(WeatherStation));
    int erro = 0;
    meteoData->estacion = AddStation(stationFile, stationName, &erro);     //Encher cada elemento do vector de WeatherStation coa estrutura Station
    meteoData->vector = AddDataVector(stationFile, &erro);
    
    switch (erro){
        case 2:
            SafeExit(vector, readFiles+1, erro, "\nErro na lectura dos datos de estación");
            break;
        case 3:
            SafeExit(vector, readFiles+1, erro, "\nErro na lectura dos datos meteorolóxicos");
            break;
    }        
    
    fclose(stationFile);
    free(fullPath);
    return meteoData;
}

Station AddStation(FILE* stationFile, char stationName[], int *erro){
    /*
    Precondición: Pasar por referencia o arquivo aberto da estación a analizar, así como o nome da mesma e unha variable de entrada/saída para modificar no caso de erro
    AddStation(stationFile, stationName);
    Poscondición: Devolver a estrutura Station completada cos datos de posición e nome. Se se produce un erro, modificará a variable erro, pasada por referencia
    */
    Station estacion;
    strcpy(estacion.nome, stationName);      //Encher o campo name co nome do arquivo sen a extensión
    
    int n;
    char *e;
    char line[256];
    do{     //Probamos a coller os datos ata que consigamos extraer os campos de lonxitude, latitude e elevación
        e=fgets(line, 256, stationFile);
        n=sscanf(line, "\"%*d/%*d/%*d\",\"%f\",\"%f\",\"%f\"", &estacion.lonxitude, &estacion.latitude, &estacion.elevacion);       //Con ampensand porque non son punteiros nin cadeas de caracteres
        
        if (e==NULL){                               //Se chegamos ao final do arquivo e non saímos do bucle, detectamos un erro.
            *erro = 2;
            n = 3;      //Se detectamos un erro, saímos do bucle
        }
    }while (n!=3);
    fseek(stationFile, 0, SEEK_SET);        //Retornar o cursor ao comezo do arquivo
    
    return estacion;        //Devolver a estrutura Station cos datos correspondiente
}

WeatherData *AddDataVector(FILE *stationFile, int *erro){
    /*
    Precondición: Pasar un punteiro ao arquivo aberto da estación da que extraer os datos, e unha variable de entrada/saída para modificar no caso de erro
    AddDataVector(stationFile);
    Poscondición: Retornar un punteiro ao vector de estruturas WeatherData que recolle os datos meteorolóxicos en cada liña do arquivo. Se se produce un erro, modificarase a variable erro.
    */
    int size;
    size = -1;
    char line[256];
    
    while (fgets(line, 256, stationFile) != NULL) {        //Atopar o número de liñas relevantes do arquivo, menos a cabeceira
        size++;
    }
    
    fseek(stationFile, 0, SEEK_SET);        //Retornar o cursor ao comezo do arquivo, saltando a liña de cabeceira
    fgets(line, 256, stationFile);
    
    WeatherData *dataVector;
    dataVector = (WeatherData *)malloc(sizeof(WeatherData)*size);
    
    int i, n;
    char *e;
    
    i = 0;
    while ((e=fgets(line, 256, stationFile))!=NULL){
        n=sscanf(line, "\"%d/%d/%d\",\"%*f\",\"%*f\",\"%*f\",\"%f\",\"%f\",\"%f\",\"%f\",\"%f\",\"%f\"", &dataVector[i].date.mes, &dataVector[i].date.dia, &dataVector[i].date.ano, &dataVector[i].max_temp, &dataVector[i].min_temp, &dataVector[i].precipitation, &dataVector[i].wind, &dataVector[i].rel_hum, &dataVector[i].solar);
        if (n!=9){
            *erro = 3;
            e = NULL;   //Se temos erro, provocamos a condición de parada e saímos do bucle
        }
        i++;
    }
    return dataVector;
}

Extreme *FindExtreme(WeatherStation *vector, int vectorSize, Fecha fecha1, Fecha fecha2){
    /*
    Precondición: Pasar por referencia o vector de WeatherStations contendo todos os datos recollidos, así como o tamaño do vector e dúas datas existentes e DAS QUE SE TEÑA REXISTRO (en orde cronolóxico)
    FindExtreme(vector, vectorSize, fecha1, fecha2);
    Poscondición: Retornar un vector de dous elementos Extreme cos datos da temperatura máxima e mínima rexistradas no intervalo de datas dadas
    */
    int i, n;
    Extreme *max, *min;
    
    Extreme *result;
    result = (Extreme *)malloc(sizeof(Extreme)*2);
    max = &result[0];       //Utilizar punteiros para acortar o código
    min = &result[1];
    
    for (i = 0; i < vectorSize; i++){
        n = 0;
        while ((vector[i].vector[n].date.ano != fecha1.ano) || (vector[i].vector[n].date.mes != fecha1.mes) || (vector[i].vector[n].date.dia != fecha1.dia)){
            n++;    //Buscar a primeira data do intervalo dado
        }
        if (i == 0){        //Se é o primeiro ficheiro, os datos dos extremos son os datos da propia estación
            max->valor = vector[i].vector[n].max_temp;
            max->estacion = vector[i].estacion;       
            max->fecha = vector[i].vector[n].date;

            min->valor = vector[i].vector[n].min_temp;
            min->estacion = vector[i].estacion;
            min->fecha = vector[i].vector[n].date;
        }
        
        while ((vector[i].vector[n].date.ano != fecha2.ano) || (vector[i].vector[n].date.mes != fecha2.mes) || (vector[i].vector[n].date.dia != fecha2.dia+1)){
            //Para o resto de estacións e días ata o último do intervalo, comparamos os valores de temperatura, e se son máis extremos que os anteriores os reemplazamos
            //Ten que incluír o último día do intervalo
            //No caso de ter datos idénticos, priorizar o máis novo
            if (vector[i].vector[n].max_temp>=max->valor){
                max->valor = vector[i].vector[n].max_temp;
                max->estacion = vector[i].estacion;
                max->fecha = vector[i].vector[n].date;
            }
            if (vector[i].vector[n].min_temp<=min->valor){
                min->valor = vector[i].vector[n].min_temp;
                min->estacion = vector[i].estacion;       
                min->fecha = vector[i].vector[n].date;
            }
            n++;
        }
    }
    return result;
}

float *FindAverage(WeatherStation *vector, int vectorSize, Fecha fecha){
    /* 
    Precondición: Pasar un punteiro ao vector de estruturas WeatherStation con todos os datos, o tamaño dese vector e a fecha da que calcular a media, que TEN QUE EXISTIR E ESTAR DENTRO DO RANGO DE DATOS
    FindAverage(vector, vectorSize, fecha);
    Poscondicón: Devolver un punteiro a un vector de dous números, que recollan a media de temperaturas máximas e mínimas.
    */
    int i, n;
    float max, min;
    max = 0;
    min = 0;
    i = 0;
    n = 0;
    while ((vector[i].vector[n].date.ano != fecha.ano) || (vector[i].vector[n].date.mes != fecha.mes) || (vector[i].vector[n].date.dia != fecha.dia)){
        n++;        //Buscar a posición da data introducida, que se manterá en todos as estacións
    }
    
    for (i = 0; i < vectorSize; i++){
        max += vector[i].vector[n].max_temp;
        min += vector[i].vector[n].min_temp;
    }
    float *result;
    result = (float *)malloc(sizeof(float)*2);
    result[0] = max/i;
    result[1] = min/i;
    
    return result;
}