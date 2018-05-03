#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <math.h>
#include <pthread.h>
#include <semaphore.h>
#include "libfractal/fractal.h"
#include "fractstack/fractstack.h"
#include "prodcons/prodcons.h"
#include "main.h"


int d_position = 0; // Determines whether all .bmp files need to be generated.
int max_threads_position = 0; // Determines whether the user has set a maximum number of threads to be used for computing the value of the fractals.
int hyphen_position = 0; // Determines whether the user is gonna enter fractals from the command line.

int STACK_SIZE;

int main(int argc, const char *argv[])
{
    // Possible options for the program call.
    const char d_arg[3] = "-d";
    const char max_threads_arg[13] = "--maxthreads";
    const char hyphen_arg[2] = "-";

    int max_threads = 7; // Test to find optimal number of threads.

    int number_input_files = 0; // Number of input files (therefore including command line input as an "input file", if input is to be read from there).

    int i;
    // Stop iterating once the loop reaches the output file which is always last or when the three modifiers have been set already, therefore not needing to iterate any longer.
    for (i = 1; i < argc - 1; ++i)
    {
        // If one of the input files is -, the user will be inputting fractals from the command line (possibly among other input methods).
        if (strcmp(argv[i], hyphen_arg) == 0)
        {
            ++number_input_files;
            hyphen_position = i;
        }
        // [-d] is an argument determining whether all the input fractals need to be transformed into .bmp files by setting the value of the generateAll boolean.
        else if (strcmp(argv[i], d_arg) == 0)
        {
            d_position = i;
        }
        // [--maxthreads n] determines the maximal number of threads the program is allowed to use.
        // If there is a specification for the maximal number of threads, but the value of n is less than one, one thread is used by default. If there is no specification at all the default value is seven, because that number seems to be optimal.
        else if (strcmp(argv[i], max_threads_arg) == 0)
        {
            max_threads = fmax(1, (int) strtol(argv[i + 1], (char **) NULL, 10));
            max_threads_position = i;
            ++i; // Skip the n parameter.
        }
        // If the argument isn't a modifier, it must be the name of an input file.
        else
        {
            ++number_input_files;
        }
    }

    STACK_SIZE = fmax(max_threads, number_input_files) + 1;

    char** input_files;
    input_files = malloc(number_input_files * sizeof(char *));
	if (input_files == NULL)
	{
		printf("Error with input_files malloc in main. \n");
	}

    int j = 0;
    for (i = 1; i < argc - 1 && j < number_input_files; ++i)
    {
        // If the argument is not one of the modifiers, it is an input file.
        if (i != d_position && i != max_threads_position  && i != max_threads_position + 1 && i != hyphen_position)
        {
            input_files[j] = malloc((strlen(argv[i]) + 1) * sizeof(char));
			if (input_files[j] == NULL)
			{
				printf("Error with input_files[j] malloc in main. \n");
			}
            strcpy(input_files[j], argv[i]);
			++j;
        }
    }

	int error = init(STACK_SIZE, max_threads);
	if (error != 0)
	{
		return EXIT_FAILURE;
	}

    pthread_t producer_threads[number_input_files]; // One producer thread for every input file.
    pthread_t consumer_threads[max_threads]; // The number of consumer threads is either set by the program call or by a default value defined by DEFAULT_NUMBER_OF_THREADS.

	printf("There are %d files (and thus producer threads). \n", number_input_files);
	printf("There are %d consumer threads. \n", max_threads);

    // Creating producer threads (one for every file).
    for (i = 0; i < number_input_files; ++i)
    {
        int err = pthread_create(&(producer_threads[i]), NULL, read_file_input, input_files[i]);
        if (err != 0)
		{
            perror("pthread_create");
        }
		else
		{
			printf("Creating producer thread number %d. \n", i);
		}
    }

    // Creating consumer threads.
    for (i = 0; i < max_threads; ++i)
	{
        int err = pthread_create(&(consumer_threads[i]), NULL, compute_fractal, NULL);
        if (err != 0)
		{
            perror("pthread_create");
        }
		else
		{
			printf("Creating consumer thread number %d. \n", i);
		}
    }

	// If the console input option was activated, read input from the console.
	if (hyphen_position != 0)
	{
		read_console_input();
	}



    // Join producer threads.
    for (i = number_input_files - 1; i >= 0; --i)
	{
        int err = pthread_join(producer_threads[i], NULL);
        if (err != 0)
		{
            perror("pthread_join");
        }
		else
		{
			printf("Joining producer thread number %d. \n", i);
		}
    }

	kill(max_threads);

    // Join consumer threads.
    for (i = max_threads - 1; i >= 0; --i) {
        int err = pthread_join(consumer_threads[i], NULL);
        if (err != 0)
		{
            perror("pthread_join");
        }
		else
		{
			printf("Joining consumer thread number %d. \n", i);
		}
    }

	// If the [-d] option wasn't selected, the only the best fractal needs to be converted into a bmp file.
	if (d_position == 0)
	{
		printf("The fractal with the highest average number of iterations was %s \n", fractal_get_name(best_fractal));
		write_bitmap_sdl(best_fractal, fractal_get_name(best_fractal));
	}

	destroy();
	pthread_mutex_destroy(&best_mutex);
	fractal_free(best_fractal);
	return EXIT_SUCCESS;
}
