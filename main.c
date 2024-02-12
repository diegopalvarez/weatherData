/* 
 * File:   main.c
 * Author: Diego Pérez Álvarez
 * Created on 11 de diciembre de 2023, 15:40
 */

#include <stdio.h>
#include <stdlib.h>
#include "meteo.h"

int main(int argc, char** argv) {
    if (argc != 2){     //Check that the name of the file is given on command line
        printf("Introduce the name of the file containing the list of the files to load.\n");
        exit(5);
    }
    
    FILE *nameFile;
    if ((nameFile=fopen(argv[1], "r"))==NULL){
        printf("Error opening the file \"%s\"", argv[1]);
        exit(1);
    }
    
    WeatherStation *vector;
    vector = NULL;
    int vectorSize;
    vectorSize = 0;
    vector = CrearVector(nameFile, &vectorSize);
    fclose(nameFile);   //Close the original file once the vector has been created
    if (vector != NULL)
        printf("%d files read succesfully\n", vectorSize);        //Check if the vector has been succesfully stored
    else
        exit(6);
    
    int choice;
    printf("\nChoose the desired action:\n    (1) Find the highest and lowest temperatures between two dates.\n    (2) Find the temperatura average on a given date.\n");
    scanf("%d", &choice);
    
    if (choice == 1){
        Fecha f1, f2;
        printf("Write the first date with format dd mm aaaa: ");
        scanf("%d %d %d", &f1.dia, &f1.mes, &f1.ano);
        printf("\nWrite the second date with format dd mm aaaa: ");
        scanf("%d %d %d", &f2.dia, &f2.mes, &f2.ano);

        if (DataValue(f1) == 1 || DataValue(f2)==1){    //Check if the date given is correct and within the data range
            SafeExit(vector, vectorSize, 4, "Invalid dates");
        } 
        Extreme *extreme;
        if (f1.ano<f2.ano || (f1.ano==f2.ano && f1.mes<f2.mes) ||(f1.ano==f2.ano && f1.mes==f2.mes && f1.dia<f2.dia))
            extreme = FindExtreme(vector, vectorSize, f1, f2);      //Sort the dates in chronological order
        else
            extreme = FindExtreme(vector, vectorSize, f2, f1);
        
        printf("\nThe highest temperature in the date range given is: %f. It happened on the station %s, on the day %d/%d/%d", extreme[0].valor, extreme[0].estacion.nome, extreme[0].fecha.dia, extreme[0].fecha.mes, extreme[0].fecha.ano);
        printf("\nAThe lowest temperature in the date range given is: %f. it happened on the station %s, on the day %d/%d/%d\n", extreme[1].valor, extreme[1].estacion.nome, extreme[1].fecha.dia, extreme[1].fecha.mes, extreme[1].fecha.ano);
        
        free(extreme);
    }
    else if (choice == 2){
        Fecha f;
        float *media;
        printf("Write the date with format dd mm aaaa: ");
        scanf("%d %d %d", &f.dia, &f.mes, &f.ano);
        if (DataValue(f) == 1){
            SafeExit(vector, vectorSize, 5, "Invalid date");        //Check if the date given is correct and within the data range
        } 
        media = FindAverage(vector, vectorSize, f);
        
        printf("\nThe average highest temperatura on the date given is: %f", media[0]);
        printf("\nThe average lowest temperatura on the date given is: %f\n", media[1]); 
        
        free(media);
    }
    else
        printf("Incorrect option");
    
    SafeExit(vector, vectorSize, 0, "END");
    return (EXIT_SUCCESS);
}