#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <math.h>
#include "fractal.h"

bool generateAll = false; // Determines whether all .bmp files need to be generated
bool commandLineInput = false; // Determines whether the user is gonna enter fractals from the command line

pthread_mutex_t mutex;
sem_t empty;
sem_t full;

int main(int argc, const char *argv[])
{
    /* TODO */
    
    const char dArg[3] = "-d";
    const char maxThreadsArg[13] = "--maxthreads";
    
    int searchIndex = 1; // Index to search for arguments in argv
    int maxThreads = 7; // Test for optimal number of threads
    
    int maxAverage = 0; // contient la valeur maximal de la moyenne des valeurs de chaque point
    
    
    
    // [--maxthreads n] can be the second and third arguments (first argument being the name of the program) or the third and fourth (because -d might take up one spot)
    // If there is a specification for the maximal number of threads, but the value of n is less than one, one thread is used by default
    if (argc > 2 && strcmp(argv[1], maxThreadsArg) == 0)
    {
        maxThreads = fmax(1, (int) strtol(argv[2], (char **)NULL, 10));
        searchIndex += 2;
    }
    else if (argc > 3 && strcmp(argv[2], maxThreadsArg) == 0)
    {
        maxThreads = fmax(1, (int) strtol(argv[3], (char **)NULL, 10));
        searchIndex += 2;
    }
    

    
    // [-d] can be the second argument (first argument being the name of the program) or the fourth (because [--maxthreads n] might take up two spots)
    if ((argc > 2 && strcmp(argv[1], dArg) == 0) ||
        (argc > 4 && strcmp(argv[3], dArg) == 0))
    {
        generateAll = true;
        searchIndex++;
    }
    
    
    
    // If one of the input files is -, the user will be inputting fractals from the command line
    int i;
    for (i = searchIndex; i < argc - 1; ++i){
        if (strcmp(argv[i], "-") == 0)
        {
            commandLineInput = true;
            break;
        }
    }
    
    
    
    struct fractal *buffer[maxThreads+1]; //buffer pour stocker les fractals
    int numberFile = argc-1-searchIndex; //nombre de fichiers qu'on aura
    pthread_t threadRead[numberFile]; //nombre de thread qu'on lance par fichier
    pthread_t threadCount[maxThreads]; //nombre de thread qu'on lance par fichier


    
    //initialisation de mutex;
    pthread_mutex_init(&mutex, NULL);
    sem_init(&empty, 0 , maxThreads+1);
    sem_init(&full, 0 , 0);
    
    //creation des threads, un thread par fichier;
    for(int i=0;i<numberFile;i++){
        err=pthread_create(&(threadRead[i]),NULL,&read_file,argc[searchIndex+i]);
        if(err!=0){
            error(err,"pthread_create");
        }
    }
    
    
    
    //creation des threads de calcules;
    for(int i=0;i<maxThreads;i++){
        err=pthread_create(&(threadCount[i]),NULL,&count,NULL);
        if(err!=0){
            error(err,"pthread_create");
        }
    }
    
    
    
    //join thread de lecture
    for(int i=numberFile-1;i>=0;i--) {
        err=pthread_join(threadRead[i],NULL);
        if(err!=0){
            error(err,"pthread_join");
        }
    }
    
    //join thread de clacule
    for(int i=maxThreads-1;i>=0;i--) {
        err=pthread_join(threadCount[i],NULL);
        if(err!=0){
            error(err,"pthread_join");
        }
    }
    
    
    
    return 0;
}
