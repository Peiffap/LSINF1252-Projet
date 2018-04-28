#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <math.h>
#include <pthread.h>
#include <semaphore.h>
#include "libfractal/fractal.h"

#define NUMBER_OF_THREADS 7

bool generateAll = false; // Determines whether all .bmp files need to be generated.
bool commandLineInput = false; // Determines whether the user is gonna enter fractals from the command line.


pthread_mutex_t mutex;
sem_t empty;
sem_t full;

int main(int argc, const char *argv[])
{
    /* TODO */
    
    const char dArg[3] = "-d";
    const char maxThreadsArg[13] = "--maxthreads";
    const char hyphenArg[2] = "-";
    
    int maxThreads = NUMBER_OF_THREADS; // Test for optimal number of threads.
    bool assignedMaxThreads = false;
    
    int nInputFiles = 0; // Number of input files (therefore including command line input as an " input file", if input is to be read from there).
    
    int i;
    // Stop iterating once the loop reaches the output file which is always last or when the 3 modifiers have been set already, therefore not needing to iterate any longer.
    for (i = 1; i < argc - 1 && (!commandLineInput && !generateAll && !assignedMaxThreads); ++i)
    {
        // If one of the input files is -, the user will be inputting fractals from the command line (possibly among other input methods).
        if (strcmp(argv[i], hyphenArg) == 0)
        {
            commandLineInput = true;
            ++nInputFiles;
        }
        // [-d] is an argument determining whether all the input fractals need to be transformed into .bmp files by setting the value of the generateAll boolean.
        else if (strcmp(argv[i], dArg) == 0)
        {
            generateAll = true;
        }
        // [--maxthreads n] determines the maximal number of threads the program is allowed to use.
        // If there is a specification for the maximal number of threads, but the value of n is less than one, one thread is used by default. If there is no specification at all the default value is 7, because that number seems to be optimal.
        else if (strcmp(argv[i], maxThreadsArg) == 0)
        {
            maxThreads = fmax(1, (int) strtol(argv[i + 1], (char **) NULL, 10));
            ++i;
            assignedMaxThreads = true;
        }
        // If the argument isn't a modifier, it must be an input file.
        else
        {
            ++nInputFiles;
        }
    }
    
    nInputFiles += argc - i; // Add eventual files that the for loop didn't reach because all modifiers were found.
    
    struct fractal *buffer[maxThreads + 1]; // Buffer to store fractals.
    pthread_t threadRead[nInputFiles]; // One thread for every input file.
    pthread_t threadCount[maxThreads]; // Threads per file.


    
    // Initialising mutexes and semaphores.
    pthread_mutex_init(&mutex, NULL);
    sem_init(&empty, 0, maxThreads + 1);
    sem_init(&full, 0, 0);
    
    // Creating threads (one for every file).
    for(i = 0; i < nInputFiles; ++i)
    {
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
    for(int i=nInputFiles-1;i>=0;i--) {
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
