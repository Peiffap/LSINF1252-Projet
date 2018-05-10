
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
	pthread_mutex_t best_fractValue; // Mutex to control access to the best fractal value.
	
	int max_threads_position = 0; // Determines whether the user has set a maximum number of threads to be used for computing the value of the fractals.
	int hyphen_position = 0; // Determines whether the user is gonna enter fractals from the command line.

	int STACK_SIZE = 20; // Default value for the number of places in the buffer.

    int max_threads = 7; // Test to find optimal number of threads.

    int number_files = 0; // Number of input files (therefore including command line input as an "input file", if input is to be read from there).

	int has_hyphen = 0; // 1 if the console is one of the places from which to read input, 0 otherwise.

	size_t lenbis = strlen(argv[0]) - 6;
	char *path = malloc(lenbis * sizeof(char));
	strncpy(path, argv[0], lenbis);

    int i;
    // Stop iterating once the loop reaches the output file which is always last.
    for (i = 1; i < argc - 1; ++i)
    {
        // If one of the input files is -, the user will be inputting fractals from the command line (possibly among other input methods).
        if (strcmp(argv[i], "-") == 0)
        {
            ++number_files;
            hyphen_position = i;
			has_hyphen = 1;
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
            ++number_files;
			// printf("%s \n", argv[i]);
        }
    }

	if (number_files == 1)
	{
		printf("There is 1 input file. \n");
	}
	else
	{
		printf("There are %d input files. \n", number_files);
	}

	if (max_threads >= number_files)
	{
		STACK_SIZE = max_threads + 1;
	}
	else
	{
		STACK_SIZE = number_files + 1;
	}

	number_files -= has_hyphen;

	// printf("number_files : %d. \n", number_files);

    char *files[number_files];

    int j = 0;
    for (i = 1; i < argc - 1 && j < number_files; ++i)
    {
		// printf("i : %d. \n", i);
        // If the argument is not one of the options, it is an input file.
		if (max_threads_position == 0)
		{
			if (i != d_position && i != hyphen_position)
	        {
	            files[j] = strdup(argv[i]);
				// printf("files[%d] : %s. \n", j, files[j]);
				++j;
	        }
		}
		else
		{
			if (i != d_position && i != max_threads_position && i != max_threads_position + 1 && i != hyphen_position)
	        {
	            files[j] = strdup(argv[i]);
				// printf("files[%d] : %s. \n", j, files[j]);
				++j;
	        }
		}
    }

	int error = init(STACK_SIZE, max_threads);
	if (error != 0)
	{
		return EXIT_FAILURE;
	}

    pthread_t producer_threads[number_files]; // One producer thread for every input file.
    pthread_t consumer_threads[max_threads]; // The number of consumer threads is either set by the program call or by a default value.

	if (number_files == 1)
	{
		printf("There is 1 producer thread. \n");
	}
	else
	{
		printf("There are %d producer threads. \n", number_files);
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
    for (i = 0; i < number_files; ++i)
    {
        error = pthread_create(&(producer_threads[i]), NULL, &read_file_input, files[i]);
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
        error = pthread_create(&(consumer_threads[i]), NULL, &compute_fractal, (void *) path);
        if (error != 0)
		{
            perror("pthread_create");
        }
		else
		{
			printf("Creating consumer thread number %d. \n", i);
		}
    }

	char *fractal_line = malloc((LINE_LENGTH + 1) * sizeof(char)); // This variable stores a line the user typed in, and describes a fractal. The length is defined so that the maximal input lengths for the different fractal parameters are accepted.

	int tmp = hyphen_position;
	// If the console input option was activated, read input from the console. As long as the user wants to keep entering fractals through standard input, the thread reading from the console keeps waiting for input.
	while (hyphen_position)
	{
		// Ask user to enter a fractal and store the result in fractal_line.
        printf("Please enter a fractal under the following format : name height width a b. \n");

		size_t len;

		int read = getline(&fractal_line, &len, stdin);

		if (read == -1)
		{
			printf("Error with getline in read_console_input. \n");
			hyphen_position = 0;
		}

		// printf("This was the entry : %s", fractal_line);

        push(line_to_fractal(fractal_line)); // Adds the newly read fractal to the stack.

        // Asks the user if they want to keep entering fractals.
        printf("Enter 1 if you wish to stop entering fractals, just hit enter if you wish to keep going. \n");
		read = getline(&fractal_line, &len, stdin);
		if (read == -1)
		{
			printf("Error with getline in read_console_input. \n");
			hyphen_position = 0;
		}
        if (strtol(fractal_line, (char **) NULL, 10) == 1)
		{
			hyphen_position = 0;
		}
	}
	free(fractal_line);
	hyphen_position = tmp;

    // Join producer threads.
    for (i = 0; i < number_files; ++i)
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

	// If the [-d] option wasn't selected, the only the best fractal needs to be converted into a bmp file.
	if (d_position == 0)
	{
		//print all best fractals "fractal_name.bmp" in outputs
		pthread_mutex_lock(&best_fractValue);
		best_fractal *run=headBestFractal;
		
		printf("Run value %s. \n", fractal_get_name(run->f));

		while (headBestFractal != NULL)
		{
			
			size_t len = strlen(fractal_get_name(headBestFractal->f));
			char *temp = malloc((len + 1 + 5 + 9 + lenbis) * sizeof(char));
			strncpy(temp, fractal_get_name(headBestFractal->f), len + 1);
			prepend(temp, "outputs/");
			prepend(temp, path);
			printf("%s \n", temp);
			
			
			write_bitmap_sdl(headBestFractal->f, strcat(temp, ".bmp"));
			printf("The fractal with the highest average number of iterations was %s. \n", fractal_get_name(headBestFractal->f));
			run = headBestFractal;
			headBestFractal=headBestFractal->next;
			free(run->f);
			free(run);
			free(temp);
		}
		pthread_mutex_unlock(&best_fractValue);
	}

	free(path);

	destroy();
	pthread_mutex_destroy(&best_mutex);

	for (i = 0; i < number_files; ++i)
	{
		free(files[i]);
	}

	return EXIT_SUCCESS;
}
