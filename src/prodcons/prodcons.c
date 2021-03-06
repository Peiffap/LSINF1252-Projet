#define LINE_LENGTH 256

#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include "../libfractal/fractal.h"
#include "prodcons.h"
#include "../fractstack/fractstack.h"
#include "../main.h"

pthread_mutex_t best_mutex; // Mutex to control access to the best fractal value.
pthread_mutex_t duplicate; // Mutex to control access to the linked list "check_list
//struct fractal *best_fractal = NULL; // Fractal with highest average number of iterations.
int d_position;
static double best_avg = 0.0;

static check_list *head_duplicate = NULL;
struct best_fractal *headBestFractal = NULL;


/**
* Producer function that reads input from a file, line per line. Lines starting with either a newline character, an octothorpe or a space are ignored.
*
* @param file_name a string containing the name of the file where the fractal is stored.
*/
void *read_file_input(void *file_name)
{
	char *file_name_str = (char *) file_name;

	char *fractal_line = malloc((LINE_LENGTH + 1) * sizeof(char)); // This variable stores a line and describes a fractal. The length is defined so that the maximal input lengths for the different fractal parameters are accepted.
	if (fractal_line == NULL)
	{
		printf("Error with malloc in read_file_input. \n");
	}

	FILE *file = NULL;
	// printf("file_name_str : %s. \n", file_name_str);
	file = fopen(file_name_str, "r"); // Opens the file specified by file_name with read permission.
	if (file == NULL)
	{
		printf("Error with fopen during file opening : %s. \n", strerror(errno));
	}
	else
	{
		// As long as the file has more lines with fractals, the function should keep reading them.
		while (fgets(fractal_line, LINE_LENGTH, file) != NULL)
		{
			// If the line doesn't start with a newline character, a space or an octothorpe, it describes a fractal and should be read accordingly.
			if (*fractal_line != '\n' && *fractal_line != '#' && *fractal_line!= ' ')
			{
				struct fractal *new_fractal = line_to_fractal(fractal_line);
				if (new_fractal != NULL) {
					push(new_fractal); // Convert the line to a pointer to a fractal struct and add the fractal to the stack.
				}
			}
		}
		fclose(file); // Closes the file after reading it.
	}
	free(fractal_line);
	pthread_exit(NULL);
}

/**
* Producer function that reads input from the console, line per line. Lines starting with either a newline character, an octothorpe or a space are ignored.
*
* @param hyphen_position an integer determining whether to read input from the console or not.
*/
void read_console_input(int hyphen_position)
{
	char *fractal_line = malloc((LINE_LENGTH + 1) * sizeof(char)); // This variable stores a line the user typed in, and describes a fractal. The length is defined so that the maximal input lengths for the different fractal parameters are accepted.

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
		// If the line doesn't start with a newline character, a space or an octothorpe, it describes a fractal and should be read accordingly.
		if (*fractal_line != '\n' && *fractal_line != '#' && *fractal_line!= ' ')
		{
			push(line_to_fractal(fractal_line)); // Adds the newly read fractal to the stack.
		}

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
	char *name = malloc(sizeof(char) * 65); // The longest the name field can be is sixty-four characters, without taking into account the null terminator.

	if (name == NULL)
	{
		printf("Error with malloc in line_to_fractal. \n");
	}

	int err = sscanf(line, "%s %d %d %lf %lf", name, &w, &h, &a, &b);
	// If the sscanf call is succesful, it should return five, since it should assign five values. If the number of assigned values isn't equal to five, a problem occured.

	// printf("In the scan || name : %s, w : %d, h : %d, a : %lf, b : %lf. \n", name, w, h, a, b);

	if (err != 5)
	{
		printf("Error in sscanf in line_to_fractal. \n");
		free(name);
		return NULL;
	}

	pthread_mutex_lock(&duplicate);
	// Iterate over list to see if the fractal already exists
	struct check_list *run = head_duplicate;
	while (run != NULL)
	{
		// Returns 0 if the fractal already exists.
		if (strcmp(run->val, name) == 0)
		{
			free(name);
			pthread_mutex_unlock(&duplicate);
			return NULL;
		}
		run = run->next;
	}

	// Add the fractal to the list.
	struct check_list *new_name = malloc(sizeof(struct check_list));
	if (new_name == NULL)
	{
		printf("Error with malloc in check_list add. \n");
		free(name);
		return NULL;
	}

	new_name->val = name;
	new_name->next = head_duplicate;
	head_duplicate = new_name;

	pthread_mutex_unlock(&duplicate);

	struct fractal *f = fractal_new(name, w, h, a, b);
	return f;
}

/**
* Prepends t into s. Assumes s has enough space allocated.
*/
void prepend(char* s, const char* t)
{
	size_t len = strlen(t);
	size_t i;

	memmove(s + len, s, strlen(s) + 1);

	for (i = 0; i < len; ++i)
	{
		s[i] = t[i];
	}
}

/**
* Computes the values of every pixel for a fractal taken from the stack, stores them in an array and stores the average value in one of the fractal's attributes.
*/
void *compute_fractal(void *p)
{
	char *path = p;
	while (1)
	{
		struct fractal *fract = pop();

		int height = fractal_get_height(fract);
		int width = fractal_get_width(fract);
		double totalIterations = 0.0;

		int i, j;
		for (i = 0; i < width; ++i)
		{
			for (j = 0; j < height; ++j)
			{
				fractal_compute_value(fract, i, j); // Computes the number of iterations for a given pixel of the fractal and stores this value in the values array of the fractal.
				// printf("x : %d || y : %d || val : %d. \n", i, j, fractal_get_value(fract, i, j));
				totalIterations += fractal_get_value(fract, i, j); // Sums up the total iterations using the number of iterations set in the previous line.
			}
		}

		double avg = totalIterations / (width * height); // Computes the average number of iterations for a given fractal.

		// If d_position isn't equal to zero then it means the [-d] was present and that a bmp file should be generated for every fractal.
		if (d_position != 0)
		{
			size_t len = 64;
			char *temp = malloc((len + 4 + 8 + strlen(path) + 1) * sizeof(char));
			// printf("Currently computing %s. \n", fractal_get_name(fract));
			const char *t = fractal_get_name(fract);
			strncpy(temp, t, len);
			prepend(temp, "outputs/");
			prepend(temp, path);
			// printf("%s \n", temp);
			write_bitmap_sdl(fract, strcat(temp, ".bmp"));
			free(temp);
		}
		pthread_mutex_lock(&best_mutex);

		// If this fractal has a higher or equal average than the current high score, then this fractal should become the new record holder.
		if(avg >= best_avg)
		{
			printf("A fractal with a higher/equal average (%f >= %f) has been found. \n", avg, best_avg);
			//If this fractal has a higher average, free stack
			if (avg != best_avg)
			{
				struct best_fractal* tmp;

				while (headBestFractal != NULL)
				{
					tmp = headBestFractal;
					headBestFractal = headBestFractal->next;
					fractal_free(tmp->f);
					free(tmp);
				}
			}
			//push new best fractal in the stack
			best_avg = avg;
			best_fractal *new_node = malloc(sizeof(struct best_fractal));

			if (new_node == NULL)
			{
				printf("Error with malloc in pushBestFractal. \n");
			}

			new_node->f = fract;
			new_node->next = headBestFractal;
			headBestFractal = new_node;
		}
		else
		{
			fractal_free(fract);
		}
		pthread_mutex_unlock(&best_mutex);
	}
}
