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

/**
 * Initialises the stack and the killer nodes.
 *
 * @param size is the size of the stack.
 * @param max_thread is the maximum number of computing threads.
 *
 * @return 0 if initialisation succesful, -1 if not.
 */
int init(int size, int max_thread);

/**
 * Kills the consumer threads.
 *
 * @params max_thread the number of consumer threads.
 */
void kill(int max_thread);

/**
 * Destroy mutexes and semaphores.
 */
void destroy();

#endif
