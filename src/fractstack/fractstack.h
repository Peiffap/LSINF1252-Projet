#ifndef _PRODCONS_H
#define _PRODCONS_H

/**
 * Adds a fractal to the stack.
 *
 * @param f is a fractal to be added to the stack.
 *
 * @return If the fractal is succesfully pushed onto the stack, returns 0. If not, the returned value is -1.
 */
int push(struct fractal *f);

/**
 * Removes and returns a fractal from the stack. If the fractal is NULL, the thread is killed.
 *
 * @return the most recently added fractal on the stack.
 */
struct fractal *pop();

#endif
