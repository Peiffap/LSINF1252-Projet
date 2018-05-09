#ifndef _PRODCONS_H
#define _PRODCONS_H

extern struct fractal *best_fractal;
extern pthread_mutex_t best_mutex;

typedef struct check_list {
    char *val;
    struct check_list *next;
} check_list;

/**
 * Producer function that reads input from a file, line per line. Lines starting with either a newline character, an octothorpe or a space are ignored.
 *
 * @param file_name a string containing the name of the file where the fractal is stored.
 */
void *read_file_input(void *file_name);

/**
 * This function takes a string describing a fractal as input and returns the fractal described by that line.
 *
 * @param line is a string describing a fractal. The order in which the fractal's attributes should be is name-height-width-a-b, where the hyphens should be replaced by regular spaces.
 *
 * @return NULL if the function encounters an error, otherwise a pointer to a fractal struct with the correct attributes.
 */
struct fractal *line_to_fractal(const char *line);

/**
 * Computes the values of every pixel for a fractal taken from the stack, stores them in an array and stores the average value in one of the fractal's attributes.
 */
void *compute_fractal();

#endif
