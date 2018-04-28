#define DEFAULT_NUMBER_OF_THREADS 7
#define BUFFER_SIZE (fmax(maxThreads, nInputFiles) + 1)
#define _GNU_SOURCE
#define LINE_LENGTH 130
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <math.h>
#include <pthread.h>
#include <semaphore.h>
#include "libfractal/fractal.h"

pthread_mutex_t mutex;
sem_t empty; // Determines whether the consumer threads can do calculations on a fractal from the buffer.
sem_t full; // Determines whether the producer threads can add another thread to the buffer.

int dPosition = 0; // Determines whether all .bmp files need to be generated.
int maxThreadsPosition = 0;
int hyphenPosition = 0; // Determines whether the user is gonna enter fractals from the command line.

fractal *buffer[BUFFER_SIZE]; // Buffer to store fractals.

int nAddedFractals = 0; // Number of fractals that have been added to the buffer.

int main(int argc, const char *argv[])
{
    // Possible arguments for the program call.
    const char dArg[3] = "-d";
    const char maxThreadsArg[13] = "--maxthreads";
    const char hyphenArg[2] = "-";
    
    int maxThreads = DEFAULT_NUMBER_OF_THREADS; // Test for optimal number of threads.
    
    int nInputFiles = 0; // Number of input files (therefore including command line input as an "input file", if input is to be read from there).
    
    int i;
    // Stop iterating once the loop reaches the output file which is always last or when the 3 modifiers have been set already, therefore not needing to iterate any longer.
    for (i = 1; i < argc - 1 && (dPosition == 0 || maxThreadsPosition == 0 || hyphenPosition == 0); ++i)
    {
        // If one of the input files is -, the user will be inputting fractals from the command line (possibly among other input methods).
        if (strcmp(argv[i], hyphenArg) == 0)
        {
            ++nInputFiles;
            hyphenPosition = i;
        }
        // [-d] is an argument determining whether all the input fractals need to be transformed into .bmp files by setting the value of the generateAll boolean.
        else if (strcmp(argv[i], dArg) == 0)
        {
            dPosition = i;
        }
        // [--maxthreads n] determines the maximal number of threads the program is allowed to use.
        // If there is a specification for the maximal number of threads, but the value of n is less than one, one thread is used by default. If there is no specification at all the default value is 7, because that number seems to be optimal.
        else if (strcmp(argv[i], maxThreadsArg) == 0)
        {
            maxThreads = fmax(1, (int) strtol(argv[i + 1], (char **) NULL, 10));
            maxThreadsPosition = i;
            ++i; // Skip the n parameter.
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
        // If the argument is not one of the modifiers, it is an input file.
        if (i != dPosition && i != maxThreadsPosition  && i != maxThreadsPosition + 1 && i != hyphenPosition)
        {
            inputFiles[j] = malloc((strlen(argv[i]) + 1) * sizeof(char));
            strcpy(inputFiles[++j], argv[i]);
        }
    }
    
    pthread_t threadRead[nInputFiles]; // One producer thread for every input file.
    pthread_t threadCount[maxThreads]; // The number of consumer threads is either set by the program call or by a default value defined by DEFAULT_NUMBER_OF_THREADS.


    
    // Initialising mutexes and semaphores.
    pthread_mutex_init(&mutex, NULL);
    sem_init(&empty, 0, BUFFER_SIZE);
    sem_init(&full, 0, 0);
    
    // Creating threads (one for every file).
    for (i = 0; i < nInputFiles; ++i)
    {
        err = pthread_create(&(threadRead[i]), NULL, &read_file, argc[searchIndex + i]);
        if (err!=0){
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

/**
 Producer function that reads input from the console.
 */
void * readConsoleInput()
{
    char fractalLine[LINE_LENGTH]; // This variable stores a line the user typed in, and describes a fractal. The length is defined so that the maximal input lengths for the different fractal parameters are accepted.
    char *wordBuffer; // Stores one word of the input line. The longest a field can be is 64 characters (and a null terminator), for the name.
    char *beginLine = NULL;
    
    bool hasNext = true; // Determines whether the user is gonna enter another fractal.
    
    // As long as the user wants to keep entering fractals through standard input, the thread reading from the console keeps waiting for input.
    while (hasNext)
    {
        // Ask user to enter a fractal and store the result in fractalLine.
        printf("Please enter a fractal under the following format : name height width a b \n");
        fgets(fractalLine, LINE_LENGTH, stdin);
        
        struct fractal *fract = lineToFractal(fractalLine);
        
        // Make sure that the different producer threads aren't overwriting each other and thus missing fractals.
        sem_wait(&empty);
        pthread_mutex_lock(&mutex);
        addToBuffer(fract); // Adds the newly read fractal to the buffer.
        pthread_mutex_unlock(&mutex);
        sem_post(&full);
        
        // Asks the user if they want to keep entering fractals.
        printf("Would you like to enter another fractal (y/n)? \n");
        hasNext = strcasecmp(fgets(fractalLine, 2, stdin), "y");
    }
    
    hyphenPosition = 0; // The user has finished manually entering fractals.
}

/**
 * This function takes a string describing a fractal as input and returns the fractal described by that line.
 *
 * @param line is a string describing a fractal. The order in which the fractal's attributes should be is name-height-width-a-b, where the hyphens should be replaced by regular spaces.
 *
 * @return NULL if the function encounters an error, otherwise a pointer to a fractal struct with the correct attributes.
 */
struct fractal *lineToFractal(char *line){
    int h, w; // Height and width of the fractal.
    double a, b; // Real and imaginary part of the fractal's constant parameter.
    char *name = malloc(sizeof(char) * 65); // The longest the name field can be is 64 characters.
    
    if (name == NULL)
        printf("Error during name allocation in lineToFractal \n");
    int err = sscanf(line, "%s %d %d %lf %lf", name, &w, &h, &a, &b);
    if (err != 5)
        return NULL;
    struct fractal *f = fractal_new(name, w, h, a, b);
    free(name);
    return f;
}

/**
 * Adds a fractal to the buffer so that consumer threads can access it.
 *
 * @param fract the fractal to be added to the buffer.
 */
void addToBuffer (fractal fract) {
    struct fractal *runner = buffer; // Iterator to run through the buffer.
    
    int i;
    // As long as the fractal being currently iterated hasn't been computed, the iterator keeps going.
    for (i = 0; fractal_get_computed(runner) == 0 && i < BUFFER_SIZE; ++i) {
        runner++;
    }
    // Once the loop's exit condition gets reached, the runner fractal points to a previously computed fractal, which we can overwrite.
    *runner = fractal;
    ++nAddedFractals; // An extra fractal has been added.
}
