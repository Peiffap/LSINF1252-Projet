#define DEFAULT_NUMBER_OF_THREADS 7
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <math.h>
#include <pthread.h>
#include <semaphore.h>
#include "libfractal/fractal.h"

pthread_mutex_t stack_mutex;// Mutex to control stack access.
sem_t empty;// Determines whether the consumer threads can do calculations on a fractal from the stack.
sem_t full;// Determines whether the producer threads can add another thread to the stack.

int dPosition = 0;// Determines whether all .bmp files need to be generated.
int maxThreadsPosition = 0;// Determines whether the user has set a maximum number of threads to be used for computing the value of the fractals.
int hyphenPosition = 0;// Determines whether the user is gonna enter fractals from the command line.

struct fractal **stack;// stack to store fractals.

int nAddedFractals = 0;// Number of fractals that have been added to the stack.
int nComputedFractals = 0;// Number of fractals whose values have been computed.

int STACK_SIZE;

/**
 * Picks a fractal from the stack so that consumer threads can compute it.
 *
 * @return the fractal to be computed by the consumer function.
 */

struct fractal *pick_from_stack()
{
    struct fractal *runner = *stack; // Iterator to run through the stack.
    struct fractal *fract = malloc(sizeof(struct fractal)); // Returned fractal.

    int i;
    // As long as the fractal being currently iterated hasn't been computed, the iterator keeps going. Since this function should only get called when the stack is ready to receive an extra fractal, i should never reach STACK_SIZE.
    for (i = 0; fractal_get_computed(runner) != 0 && i < STACK_SIZE; ++i)
    {
        ++runner;
    }
    // Once the loop's exit condition gets reached, the runner fractal points to a non-computed fractal, which we can then compute.
    *fract = *runner;
    fractal_set_computed(runner, 1);// The chosen fractal is now (about to be) computed, so it can be considered as computed.
    return fract;
}

/**
 * Adds a fractal to the stack so that consumer threads can access it.
 *
 * @param fract the fractal to be added to the stack.
 */
void add_to_stack(struct fractal *fract)
{
    struct fractal *runner = *stack; // Iterator to run through the stack.

    int i;
    // As long as the fractal being currently iterated hasn't been computed, the iterator keeps going. Since this function should only get called when the stack is ready to receive an extra fractal, i should never reach STACK_SIZE.
    for (i = 0; fractal_get_computed(runner) == 0 && i < STACK_SIZE; ++i)
    {
        ++runner;
    }
    // Once the loop's exit condition gets reached, the runner fractal points to a previously computed fractal, which we can overwrite.
    runner = fract;
    ++nAddedFractals; // An extra fractal has been added.
}

/**
 * Computes the values of every pixel for a fractal taken from the stack, stores them in an array and stores the average value in one of the fractal's attributes.
 */
void *compute_fractal()
{
    struct fractal *fract = malloc(sizeof(struct fractal));

    sem_wait(&full);
    pthread_mutex_lock(&stack_mutex);
    fract = pick_from_stack(); // Picks a non-computed fractal from the stack.
    pthread_mutex_unlock(&stack_mutex);
    sem_post(&empty);

    int height = fractal_get_height(fract);
    int width = fractal_get_width(fract);
    double totalIterations = 0.0;

    int i, j;
    for (i = 0; i < width; ++i)
    {
        for (j = 0; j < height; ++j)
        {
            fractal_compute_value(fract, i, j); // Computes the number of iterations for a given pixel of the fractal and stores this value in the values array of the fractal.
            totalIterations += fractal_get_value(fract, i, j); // Sums up the total iterations using the number of iterations set in the previous line.
        }
    }

    double avg = (double) avg / (double) (width * height); // Computes the average number of iterations for a given fractal.
    fractal_set_average(fract, avg); // Stores this average value in the fractal's average attribute.

    // If dPosition isn't equal to zero then it means the [-d] was present and that all a .bmp file should be generated for every fractal.
    if (dPosition != 0)
    {
        write_bitmap_sdl(fract, fractal_get_name(fract));
    }

    pthread_mutex_lock(&bestMutex);
    // If this fractal has a higher average than the current high score, then this fractal should become the new record holder.
    if (avg > fractal_get_average(bestFractal))
    {
        *bestFractal = *fract;
    }
    ++nComputedFractals; // An extra fractal has been computed.
    pthread_mutex_unlock(&bestMutex);

    free(fract);
    return NULL;
}

/**
 * Producer function that reads input from the console, line per line.
 */
void * read_console_input()
{
    char fractalLine[LINE_LENGTH]; // This variable stores a line the user typed in, and describes a fractal. The length is defined so that the maximal input lengths for the different fractal parameters are accepted.
    char y[2]; // Stores the user's answer when asked if they want to enter another fractal from standard input (y/n).

    bool hasNext = true; // Determines whether the user is gonna enter another fractal.

    // As long as the user wants to keep entering fractals through standard input, the thread reading from the console keeps waiting for input.
    while (hasNext)
    {
        // Ask user to enter a fractal and store the result in fractalLine.
        printf("Please enter a fractal under the following format : name height width a b. \n");
        fgets(fractalLine, LINE_LENGTH, stdin);

        struct fractal *fract = line_to_fractal(fractalLine);

        // Make sure that the different producer threads aren't overwriting each other and thus missing fractals.
        sem_wait(&empty);
        pthread_mutex_lock(&stack_mutex);
        add_to_stack(fract); // Adds the newly read fractal to the stack.
        pthread_mutex_unlock(&stack_mutex);
        sem_post(&full);

        // Asks the user if they want to keep entering fractals.
        printf("Would you like to enter another fractal (y/n)? \n");
        hasNext = strcasecmp(fgets(y, 2, stdin), "y");
    }

    hyphenPosition = 0; // The user has finished manually entering fractals.
    return NULL;
}



int main(int argc, const char *argv[])
{
    // Possible arguments for the program call.
    const char dArg[3] = "-d";
    const char maxThreadsArg[13] = "--maxthreads";
    const char hyphenArg[2] = "-";

    int maxThreads = DEFAULT_NUMBER_OF_THREADS; // Test for optimal number of threads.

    int nInputFiles = 0; // Number of input files (therefore including command line input as an "input file", if input is to be read from there).

    int i;
    // Stop iterating once the loop reaches the output file which is always last or when the three modifiers have been set already, therefore not needing to iterate any longer.
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
        // If there is a specification for the maximal number of threads, but the value of n is less than one, one thread is used by default. If there is no specification at all the default value is seven, because that number seems to be optimal.
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

    STACK_SIZE = fmax(maxThreads, nInputFiles) + 1;
    stack = malloc(STACK_SIZE * sizeof struct fractal *);

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
    pthread_mutex_init(&stack_mutex, NULL);
    sem_init(&empty, 0, STACK_SIZE);
    sem_init(&full, 0, 0);

    // Creating threads (one for every file).
    for (i = 0; i < nInputFiles; ++i)
    {
        int err = pthread_create(&(threadRead[i]), NULL, read_file_input, (void *) argv[i]);
        if (err!=0){
            perror("pthread_creat");
            exit(EXIT_FAILURE);
        }
    }



    //creation des threads de calcules;
    for(int i=0;i<maxThreads;i++){
        int err=pthread_create(&(threadCount[i]),NULL,compute_fractal,NULL);
        if(err!=0){
            perror("pthread_creat");
            exit(EXIT_FAILURE);
        }
    }



    //join thread de lecture
    for(int i=nInputFiles-1;i>=0;i--) {
        int err=pthread_join(threadRead[i],NULL);
        if(err!=0){
            perror("pthread_creat");
            exit(EXIT_FAILURE);
        }
    }

    //join thread de clacule
    for(int i=maxThreads-1;i>=0;i--) {
        int err=pthread_join(threadCount[i],NULL);
        if(err!=0){
            perror("pthread_creat");
            exit(EXIT_FAILURE);
        }
    }



    return 0;
}
