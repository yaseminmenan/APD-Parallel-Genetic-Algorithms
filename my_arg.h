#ifndef MY_ARG_H
#define MY_ARG_H

// structure for arguments passed to every thread
typedef struct _my_arg {
	int id;
	int P;
	sack_object *objects;
	int object_count;
	int generations_count;
	int sack_capacity;
	individual *current_generation;
	individual *next_generation;
	pthread_barrier_t *barrier;
} my_arg;

#endif