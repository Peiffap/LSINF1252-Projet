#include <stdlib.h>
#include <stdio.h>
#include <semaphore.h>
#include <pthread.h>
#include "../libfractal/fractal.h"
#include "fractstack.h"

static pthread_mutex_t stack_mutex;
static sem_t empty;
static sem_t full;
static node *head = NULL;

/**
 * Adds a fractal to the stack.
 *
 * @param f is a fractal to be added to the stack.
 *
 * @return If the fractal is succesfully pushed onto the stack, returns 0. If not, the returned value is -1.
 */
int push(struct fractal *f)
{
	node *new_node = malloc(sizeof node);

	if (new_node == NULL)
	{
		printf("Error with malloc in push. \n");
		return -1;
	}

	if (f == NULL)
	{
		printf("Error, NULL fractal in push. \n");
		return -1;
	}

	new_node->f = f;

	sem_wait(&empty);
	pthread_mutex_lock(&stack_mutex);
	new_node->next = head;
	head = new_node;
	pthread_mutex_unlock(&stack_mutex);
	sem_post(&full);

	return 0;
}

/**
 * Removes and returns a fractal from the stack. If the fractal is NULL, the thread is killed.
 *
 * @return the most recently added fractal on the stack.
 */
struct fractal *pop()
{
	sem_wait(&full);
	pthread_mutex_lock(&stack_mutex);
	struct fractal *f = head->f;
	node *save = head;
	head = head->next;
	pthread_mutex_unlock(&stack_mutex);
	sem_post(&empty);

	if(f == NULL)
	{
		printf("Fractal was NULL in pop; thread will get killed. \n");
		pthread_exit(NULL);
	}
	free(save);
	return f;
}


/**
 * Initialises the stack and the killer nodes.
 *
 * @param size is the size of the stack.
 * @param max_thread is the maximum number of computing threads.
 *
 * @return 0 if initialisation succesful, -1 if not.
 */
int init(int size, int max_thread)
{
	pthread_mutex_init(&stack_mutex, NULL);
	sem_init(&empty, 0, size);
	sem_init(&full, 0, 0);

	int i;
	for (i = 0; i < max_thread; ++i)
	{
		node *new_node = malloc(sizeof node);
		if (new_node == NULL){
			printf("Error with malloc in init. \n");
			return -1;
		}
		new_node->f = NULL;
		pthread_mutex_lock(&stack_mutex);
		new_node->next = head;
		head = new_node;
		pthread_mutex_unlock(&stack_mutex);
	}

	return 0;
}

/**
 * Kills the consumer threads.
 *
 * @params max_thread the number of consumer threads.
 */
void kill(int max_thread)
{
	printf("Killing consumer threads. \n");

	int i;
	for(i = 0; i < max_thread; ++i){
		sem_post(&full);
	}
}

/**
 * Destroy mutexes and semaphores.
 */
void destroy()
{
	pthread_mutex_destroy(&stack_mutex);
	sem_destroy(&empty);
	sem_destroy(&full);
}
