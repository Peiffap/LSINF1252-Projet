#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "fractal.h"

struct fractal *fractal_new(const char *name, int width, int height, double a, double b)
{
    struct fractal *newFractal = malloc(sizeof(struct fractal));
    if (newFractal == NULL)
    {
        return NULL;
    }

    char *n = malloc((strlen(name) + 1) * sizeof(char)); // Allocate space for string and null terminator.
    strcpy(n, name); // Copy the argument string into the newly allocated memory space.
    newFractal->theName = n;
    newFractal->theWidth = width;
    newFractal->theHeight = height;
    newFractal->theA = a;
    newFractal->theB = b;

    int **data; // Local array.
    data = malloc(width * sizeof(int *)); // Pointer vector.
    if (data == NULL)
    {
        return NULL;
    }
    int i;
    for (i = 0; i < width; ++i)
    {
        data[i] = malloc(height * sizeof(int)); // Allocate each entry in the array allocated above.
        if (data[i] == NULL)
        {
            return NULL;
        }
    }
    newFractal->values = data; // Point to local array.

    return newFractal;
}

void fractal_free(struct fractal *f)
{
    free(f->theName); // Free name field.

    int i;
    for (i = 0; i < f->theWidth; ++i)
    {
        free(f->values[i]); // For every allocated pointer in the pointer vector, free that pointer.
    }
    free(f->values); // Free values array.

    free((void *) f); // Free struct.
}

const char *fractal_get_name(const struct fractal *f)
{
    return f->theName;
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
    return f->theHeight;
}

int fractal_get_width(const struct fractal *f)
{
    return f->theWidth;
}

double fractal_get_a(const struct fractal *f)
{
    return f->theA;
}

double fractal_get_b(const struct fractal *f)
{
    return f->theB;
}
