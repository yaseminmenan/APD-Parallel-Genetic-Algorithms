#include <stdlib.h>
#include "genetic_algorithm_par.h"

int main(int argc, char *argv[]) {
	// array with all the objects that can be placed in the sack
	sack_object *objects = NULL;

	// number of objects
	int object_count = 0;

	// maximum weight that can be carried in the sack
	int sack_capacity = 0;

	// number of generations
	int generations_count = 0;

	// number of threads
	int P = 0;

	if (!read_input(&objects, &object_count, &sack_capacity, &generations_count, &P, argc, argv)) {
		return 0;
	}


	int i;
	pthread_t *threads;
	pthread_barrier_t barrier;
	my_arg *arguments;
	individual *current_generation, *next_generation;

	// Allocate space for variables
	pthread_barrier_init(&barrier, NULL, P);
	threads = (pthread_t*) malloc(P * sizeof(pthread_t));
	arguments = (my_arg*) malloc(P * sizeof(my_arg) + 2 * object_count * sizeof(individual));
	current_generation = (individual*) calloc(object_count, sizeof(individual));
	next_generation = (individual*) calloc(object_count, sizeof(individual));

	// Dynamically allocate the individuals of current_generation and next_generation
	for (i = 0; i < object_count; ++i) {
		// Set initial generation (composed of object_count individuals with a single item in the sack)
		current_generation[i].fitness = 0;
		current_generation[i].chromosomes = (int*) calloc(object_count, sizeof(int));
		current_generation[i].chromosomes[i] = 1;
		current_generation[i].nr_one = 1;
		current_generation[i].index = i;
		current_generation[i].chromosome_length = object_count;

		next_generation[i].fitness = 0;
		next_generation[i].chromosomes = (int*) calloc(object_count, sizeof(int));
		next_generation[i].index = i;
		next_generation[i].nr_one = 0;
		next_generation[i].chromosome_length = object_count;
	}

	// create threads
	for (i = 0; i < P; i++) {
		arguments[i].id = i;
		arguments[i].P = P;
		arguments[i].objects = objects;
		arguments[i].object_count = object_count;
		arguments[i].generations_count = generations_count;
		arguments[i].sack_capacity = sack_capacity;
		arguments[i].barrier = &barrier;
		arguments[i].current_generation = current_generation;
		arguments[i].next_generation = next_generation;

		pthread_create(&threads[i], NULL, run_genetic_algorithm, &arguments[i]);
	}

	for (i = 0; i < P; i++) {
		pthread_join(threads[i], NULL);
	}


	// free resources for old generation
	free_generation(current_generation);
	free_generation(next_generation);

	// free resources
	free(current_generation);
	free(next_generation);
	free(objects);
	free(threads);
	free(arguments);

	pthread_barrier_destroy(&barrier);

	return 0;
}
