#include <assert.h>
#include <stdlib.h>

#include "queue.h"

queue_t* queue_create()
{
	// Create a record in dynamic memory with all its bits in zero
	queue_t* queue = calloc( 1, sizeof(queue_t) );
	assert(queue);
	return queue;
}

void* queue_append(queue_t* queue, void* data)
{
	assert(queue);

	// Create the node and copy the data
	queue_node_t* node = calloc(1, sizeof(queue_node_t));
	if ( node == NULL ) return NULL;
	node->data = data;

	// Append the new node to the queue
	if ( queue_is_empty(queue) )
		queue->head = queue->tail = node;
	else
		queue->tail = queue->tail->next = node;

	// Return a pointer to the data
	++queue->count;
	return node->data;
}

void* queue_peek(queue_t* queue)
{
	assert(queue);
	assert(queue_is_empty(queue) == false);

	return queue->head->data;
}

size_t queue_count(const queue_t* queue)
{
	assert(queue);

	return queue->count;
}

bool queue_is_empty(const queue_t* queue)
{
	assert(queue);

	return queue->head == NULL;
}

void queue_destroy(queue_t* queue, bool remove_data)
{
	assert(queue);

	queue_clear(queue, remove_data);
	free(queue);
}

void queue_clear(queue_t* queue, bool remove_data)
{
	assert(queue);

	while ( ! queue_is_empty(queue) )
	{
		if ( remove_data )
			free ( queue_pop(queue) );
		else
			queue_pop(queue);
	}

	queue->head = queue->tail = NULL;
}

void* queue_pop(queue_t* queue)
{
	assert(queue);
	assert(queue_is_empty(queue) == false);

	// Get a pointer to the next node and its data
	queue_node_t* node = queue->head;
	void* data = node->data;

	// Move the head to the next node
	queue->head = queue->head->next;
	--queue->count;

	// If queue bacame empty, update the tail
	if ( queue_is_empty(queue) )
		queue->tail = NULL;

	// Release the memory of the node, not its data
	free(node);
	return data;
}
