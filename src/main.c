#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <math.h>
#include <pthread.h>
#include <semaphore.h>
#include "libfractal/fractal.h"

#define DEFAULT_NUMBER_OF_THREADS 7
#define BUFFER_SIZE (fmax(maxThreads, nInputFiles) + 1)

bool generateAll = false; // Determines whether all .bmp files need to be generated.
bool commandLineInput = false; // Determines whether the user is gonna enter fractals from the command line.


pthread_mutex_t mutex;
sem_t empty; // Determines whether the consumer threads can do calculations on a fractal from the buffer.
sem_t full; // Determines whether the producer threads can add another thread to the buffer.

int main(int argc, const char *argv[])
{
    // Possible arguments for the program call.
    const char dArg[3] = "-d";
    const char maxThreadsArg[13] = "--maxthreads";
    const char hyphenArg[2] = "-";
    
    int dPosition = 0;
    int maxThreadsPosition = 0;
    int hyphenPosition = 0;
    
    int maxThreads = DEFAULT_NUMBER_OF_THREADS; // Test for optimal number of threads.
    bool assignedMaxThreads = false; // Used as exit condition for the for loop.
    
    int nInputFiles = 0; // Number of input files (therefore including command line input as an " input file", if input is to be read from there).
    
    int i;
    // Stop iterating once the loop reaches the output file which is always last or when the 3 modifiers have been set already, therefore not needing to iterate any longer.
    for (i = 1; i < argc - 1; ++i)
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
        // If the argument isn't a modifier, it must be the name of an input file.
        else
        {
            ++nInputFiles;
        }
    }
    
    char** inputFiles;
    inputFiles = malloc(nInputFiles * sizeof(char*));
    int j = 0;
    
    for (i = 1; i < argc - 1 && j < nInputFiles; ++i)
    {
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
        // If the argument isn't a modifier, it must be the name of an input file.
        else
        {
            ++nInputFiles;
        }
    }
    
    
    struct fractal *buffer[BUFFER_SIZE]; // Buffer to store fractals.
    pthread_t threadRead[nInputFiles]; // One producer thread for every input file.
    pthread_t threadCount[maxThreads]; // The number of consumer threads is either set by the program call or by a default value defined by DEFAULT_NUMBER_OF_THREADS.


    
    // Initialising mutexes and semaphores.
    pthread_mutex_init(&mutex, NULL);
    sem_init(&empty, 0, BUFFER_SIZE);
    sem_init(&full, 0, 0);
    
    // Creating threads (one for every file).
    for(i = 0; i < nInputFiles; ++i)
    {
        err = pthread_create(&(threadRead[i]), NULL, &read_file, argc[searchIndex + i]);
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
