#include <stdlib.h>
#include "fractal.h"

struct fractal *fractal_new(const char *name, int width, int height, double a, double b)
{
    /* TODO */
    
    struct fractal *newFractal = (struct fractal *) malloc(sizeof(struct fractal));
    
    newFractal->theName = name; // No need to malloc, pointer already exists
    newFractal->theWidth = width;
    newFractal->theHeight = height;
    newFractal->theA = a;
    newFractal->theB = b;
    
    int **data; // Local array
    data = malloc(width * sizeof(int *)); // Pointer vector
    int i;
    for (i = 0; i < width; i++)
        
        //coucou
    {
        data[i] = malloc(height * sizeof(int)); // Allocate each entry in the array allocated above
    }
    newFractal->values = data; // Point to local array
    
    return newFractal;
}

void fractal_free(struct fractal *f)
{
    /* TODO */
    
    free(f->theName); // Free name field
    
    int i;
    for (i = 0; i < f->theWidth; i++)
    {
        free(f->values[i]); // For every allocated pointer in the pointer vector, free that pointer
    }
    free(values); // Free values array
    
    free(f); // Free struct
}

const char *fractal_get_name(const struct fractal *f)
{
    /* TODO */
    
    
    
    return NULL;
}

int fractal_get_value(const struct fractal *f, int x, int y)
{
    /* TODO */
    return 0;
}

void fractal_set_value(struct fractal *f, int x, int y, int val)
{
    /* TODO */
    
    f->
    
}

int fractal_get_width(const struct fractal *f)
{
    /* TODO */
    return 0;
}

int fractal_get_height(const struct fractal *f)
{
    /* TODO */
    return 0;
}

double fractal_get_a(const struct fractal *f)
{
    /* TODO */
    return 0;
}

double fractal_get_b(const struct fractal *f)
{
    /* TODO */
    return 0;
}
