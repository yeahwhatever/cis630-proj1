/**
 * Common functions shared between client and server
 *
 */

#include <stdlib.h>

#include "common.h"

/**
 * malloc wrapper to die quickly if we run out of memory
 */
void* xmalloc(unsigned int size) {
	void *ptr;

	ptr = malloc(size);

	if (ptr)
		return ptr;

	exit(255);
}
