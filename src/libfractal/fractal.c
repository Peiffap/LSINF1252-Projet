#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "fractal.h"

struct fractal *fractal_new(const char *name, int width, int height, double a, double b)
{
    struct fractal *new_fractal = malloc(sizeof(struct fractal));
    if (new_fractal == NULL)
    {
		printf("Error in fractal malloc in fractal_new. \n");
        return NULL;
    }

	size_t len = strlen(name);
	// printf("len = %zu, name : %s. \n", len, name);
    new_fractal->the_name = malloc((len + 1) * sizeof(char));
	if (new_fractal->the_name == NULL)
	{
		printf("Error in name malloc in fractal_new. \n");
		return NULL;
	}
	strncpy(new_fractal->the_name, name, len + 1);

	// printf("In fractal || name : %s. \n", new_fractal->the_name);
    new_fractal->the_width = width;
	// printf("In fractal || width : %d. \n", new_fractal->the_width);
    new_fractal->the_height = height;
	// printf("In fractal || height : %d. \n", new_fractal->the_height);
    new_fractal->the_a = a;
	// printf("In fractal || a : %lf. \n", new_fractal->the_a);
    new_fractal->the_b = b;
	// printf("In fractal || b : %lf. \n", new_fractal->the_b);

    int **data; // Local array.
    data = malloc(width * sizeof(int *)); // Pointer vector.
    if (data == NULL)
    {
		printf("Error in first array malloc in fractal_new. \n");
		free(new_fractal->the_name);
        return NULL;
    }
    int i, j;
    for (i = 0; i < width; ++i)
    {
        data[i] = malloc(height * sizeof(int)); // Allocate each entry in the array allocated above.
        if (data[i] == NULL)
        {
			printf("Error in second array malloc in fractal_new. \n");
			for (j = 0; j < i; ++j)
			{
				free(data[j]);
			}
			free(data);
            return NULL;
        }
    }
    new_fractal->values = data; // Point to local array.

    return new_fractal;
}

void fractal_free(struct fractal *f)
{
    free(f->the_name); // Free name field.

	int i;
    for (i = 0; i < f->the_width; ++i)
    {
		free(f->values[i]); // For every allocated pointer in the pointer vector, free that pointer.
    }
	free(f->values); // Free values array.

    free(f); // Free struct.
}

const char *fractal_get_name(const struct fractal *f)
{
    return strdup(f->the_name);
}

int fractal_get_value(const struct fractal *f, int x, int y)
{
    return f->values[x][y];
}

void fractal_set_value(struct fractal *f, int x, int y, int val)
{
	f->values[x][y] = val;
}

int fractal_get_height(const struct fractal *f)
{
    return f->the_height;
}

int fractal_get_width(const struct fractal *f)
{
    return f->the_width;
}

double fractal_get_a(const struct fractal *f)
{
    return f->the_a;
}

double fractal_get_b(const struct fractal *f)
{
    return f->the_b;
}
