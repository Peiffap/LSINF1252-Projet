#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include "./libfractal/fractal.h"
#include "./fractstack/fractstack.h"
#include "./prodcons/prodcons.h"
#include "main.h"


int main(int argc, const char *argv[])
{
	int d_position = 0; // Determines whether all .bmp files need to be generated.
	int max_threads_position = 0; // Determines whether the user has set a maximum number of threads to be used for computing the value of the fractals.
	int hyphen_position = 0; // Determines whether the user is gonna enter fractals from the command line.

	int STACK_SIZE;

    int max_threads = 7; // Test to find optimal number of threads.

    int number_input_files = 0; // Number of input files (therefore including command line input as an "input file", if input is to be read from there).

	int has_hyphen = 0;

    int i;
    // Stop iterating once the loop reaches the output file which is always last or when the three modifiers have been set already, therefore not needing to iterate any longer.
    for (i = 1; i < argc - 1; ++i)
    {
        // If one of the input files is -, the user will be inputting fractals from the command line (possibly among other input methods).
        if (strcmp(argv[i], "-") == 0)
        {
            ++number_input_files;
            hyphen_position = i;
			printf("The hyphen argument was argument number %d. \n", i);
        }
        // [-d] is an argument determining whether all the input fractals need to be transformed into .bmp files by setting the value of the generateAll boolean.
        else if (strcmp(argv[i], "-d") == 0)
        {
            d_position = i;
			printf("The [-d] argument was argument number %d. \n", i);
        }
        // [--maxthreads n] determines the maximal number of threads the program is allowed to use.
        // If there is a specification for the maximal number of threads, but the value of n is less than one, one thread is used by default. If there is no specification at all the default value is used.
        else if (strcmp(argv[i], "--maxthreads") == 0)
        {
            int value = (int) strtol(argv[i + 1], (char **) NULL, 10);
            if (value > 1)
            {
                max_threads = value;
            }
            else
            {
                max_threads = 1;
            }
            max_threads_position = i;
            ++i; // Skip the n parameter.
			printf("The [--maxthreads n] arguments were arguments number %d and %d. \n", i - 1, i);
        }
        // If the argument isn't a modifier, it must be the name of an input file.
        else
        {
            ++number_input_files;
			printf("%s \n", argv[i]);
        }
    }

	if (number_input_files == 1)
	{
		printf("There is 1 file. \n");
	}
	else
	{
		printf("There are %d files. \n", number_input_files);
	}

	if (max_threads >= number_input_files)
	{
		STACK_SIZE = max_threads + 1;
	}
	else
	{
		STACK_SIZE = number_input_files + 1;
	}

	number_input_files -= has_hyphen;

    char *input_files[number_input_files];

    int j = 0;
    for (i = 1; i < argc - 1 && j < number_input_files; ++i)
    {
        // If the argument is not one of the options, it is an input file.
        if (i != d_position && i != max_threads_position  && i != max_threads_position + 1 && i != hyphen_position)
        {
            input_files[++j] = strdup(argv[i]);
        }
    }

	int error = init(STACK_SIZE, max_threads);
	if (error != 0)
	{
		return EXIT_FAILURE;
	}

    pthread_t producer_threads[number_input_files]; // One producer thread for every input file.
    pthread_t consumer_threads[max_threads]; // The number of consumer threads is either set by the program call or by a default value.

	if (number_input_files == 1)
	{
		printf("There is 1 producer thread. \n");
	}
	else
	{
		printf("There are %d producer threads. \n", number_input_files);
	}
	if (max_threads == 1)
	{
		printf("There is 1 consumer thread. \n");
	}
	else
	{
		printf("There are %d consumer threads. \n", max_threads);
	}

    // Creating producer threads (one for every file).
    for (i = 0; i < number_input_files; ++i)
    {
        error = pthread_create(&(producer_threads[i]), NULL, &read_file_input, input_files[i]);
        if (error != 0)
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
        error = pthread_create(&(consumer_threads[i]), NULL, &compute_fractal, NULL);
        if (error != 0)
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
    for (i = 0; i < number_input_files; ++i)
	{
        error = pthread_join(producer_threads[i], NULL);
        if (error != 0)
		{
            perror("pthread_join");
        }
		else
		{
			printf("Joining producer thread number %d. \n", i);
		}
    }

	kill(max_threads); // Kill consumer threads.

    // Join consumer threads.
    for (i = 0; i < max_threads; ++i) {
        error = pthread_join(consumer_threads[i], NULL);
        if (error != 0)
		{
            perror("pthread_join");
        }
		else
		{
			printf("Joining consumer thread number %d. \n", i);
		}
    }

	free_stack(); // Free the stack.

	// If the [-d] option wasn't selected, the only the best fractal needs to be converted into a bmp file.
	if (d_position == 0)
	{
		printf("The fractal with the highest average number of iterations was %s. \n", fractal_get_name(best_fractal));
		int len = strlen(argv[argc - 1]) + 1;
		char *temp = malloc((len + 5) * sizeof(char));
		strncpy(temp, argv[argc - 1], len);
		write_bitmap_sdl(best_fractal, strcat(temp, ".bmp"));
		free(temp);
	}

	destroy();
	pthread_mutex_destroy(&best_mutex);
	fractal_free(best_fractal);

	for (i = 0; i < number_input_files; ++i)
	{
		free(input_files[i]);
	}

	return EXIT_SUCCESS;
}
