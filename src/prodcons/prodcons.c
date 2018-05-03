#define LINE_LENGTH 256

#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "../libfractal/fractal.h"
#include "prodcons.h"
#include "../fractstack/fractstack.h"

pthread_mutex_t bestMutex; // Mutex to control access to the best fractal value.
struct fractal *bestFractal; // Fractal with highest average number of iterations.
double highest_avg = 0.0; // Keeps track of the highest average value among all fractals.
int dPosition;

/**
 * Producer function that reads input from a file, line per line. Lines starting with either a newline character, an octothorpe or a space are ignored.
 *
 * @param fileName a string containing the name of the file where the fractal is stored.
 */
void *read_file_input(const char *fileName)
{
    FILE *file = NULL;
    char *fractalLine = malloc((LINE_LENGTH + 1) * sizeof(char)); // This variable stores a line and describes a fractal. The length is defined so that the maximal input lengths for the different fractal parameters are accepted.

	if (fractalLine == NULL)
	{
		printf("Error with malloc in read_file_input. \n");
	}

    file = fopen(fileName, "r"); // Opens the file specified by fileName with read permission.
    if (file == NULL)
    {
        printf("Error with fopen during file opening. \n");
    }
    else
    {
        // As long as the file has more lines with fractals, the function should keep reading them.
        while (fgets(fractalLine, LINE_LENGTH, file) != NULL)
        {
            // If the line doesn't start with a newline character, a space or an octothorpe, it describes a fractal and should be read accordingly.
            if (*fractalLine != '\n' && *fractalLine != '#' && *fractalLine!= ' ')
            {
                sem_wait(&empty);
                pthread_mutex_lock(&bufferMutex);
                add_to_stack(line_to_fractal(fractalLine)); // Convert the line to a pointer to a fractal struct and add the fractal to the stack.
                pthread_mutex_unlock(&bufferMutex);
                sem_post(&full);
            }
        }
        fclose(file); // Closes the file after reading it.
    }
	free(fractalLine);
	free(fileName);
    pthread_exit(NULL);
}


/**
 * This function takes a string describing a fractal as input and returns the fractal described by that line.
 *
 * @param line is a string describing a fractal. The order in which the fractal's attributes should be is name-height-width-a-b, where the hyphens should be replaced by regular spaces.
 *
 * @return NULL if the function encounters an error, otherwise a pointer to a fractal struct with the correct attributes.
 */
struct fractal *line_to_fractal(const char *line)
{
    int h, w; // Height and width of the fractal.
    double a, b; // Real and imaginary part of the fractal's constant parameter.
    const char *name = malloc(sizeof(char) * 65); // The longest the name field can be is sixty-four characters, without taking into account the null terminator.

    if (name == NULL)
    {
        printf("Error with malloc in line_to_fractal. \n");
    }

    int err = sscanf(line, "%s %d %d %lf %lf", name, &w, &h, &a, &b);
    // If the sscanf call is succesful, it should return five, since it should assign five values. If the number of assigned values isn't equal to five, a problem occured.

    if (err != 5)
    {
        return NULL;
    }
    struct fractal *f = fractal_new(name, w, h, a, b);
    free(name);
    return f;
}
