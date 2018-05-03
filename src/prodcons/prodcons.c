#define LINE_LENGTH 256

#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "../libfractal/fractal.h"
#include "prodcons.h"
#include "../fractstack/fractstack.h"

pthread_mutex_t best_mutex; // Mutex to control access to the best fractal value.
struct fractal *best_fractal; // Fractal with highest average number of iterations.
double highest_avg = 0.0; // Keeps track of the highest average value among all fractals.
int d_position;

/**
 * Producer function that reads input from a file, line per line. Lines starting with either a newline character, an octothorpe or a space are ignored.
 *
 * @param file_name a string containing the name of the file where the fractal is stored.
 */
void *read_file_input(const char *file_name)
{
    FILE *file = NULL;
    char *fractal_line = malloc((LINE_LENGTH + 1) * sizeof(char)); // This variable stores a line and describes a fractal. The length is defined so that the maximal input lengths for the different fractal parameters are accepted.

	if (fractal_line == NULL)
	{
		printf("Error with malloc in read_file_input. \n");
	}

    file = fopen(file_name, "r"); // Opens the file specified by file_name with read permission.
    if (file == NULL)
    {
        printf("Error with fopen during file opening. \n");
    }
    else
    {
        // As long as the file has more lines with fractals, the function should keep reading them.
        while (fgets(fractal_line, LINE_LENGTH, file) != NULL)
        {
            // If the line doesn't start with a newline character, a space or an octothorpe, it describes a fractal and should be read accordingly.
            if (*fractal_line != '\n' && *fractal_line != '#' && *fractal_line!= ' ')
            {
                add_to_stack(line_to_fractal(fractal_line)); // Convert the line to a pointer to a fractal struct and add the fractal to the stack.
            }
        }
        fclose(file); // Closes the file after reading it.
    }
	free(fractal_line);
	free(file_name);
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
