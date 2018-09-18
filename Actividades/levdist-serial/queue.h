#ifndef QUEUE_H
#define QUEUE_H

#include <stdbool.h>
#include <stddef.h>

/// A generic node of the queue
typedef struct queue_node
{
	/// Pointer to the user data
	void* data;
	/// Next node in the queue
	struct queue_node* next;
} queue_node_t;

/// A queue of user data elements
typedef struct
{
	/// Number of elements in the queue
	size_t count;
	/// Pointer to the first element in the queue
	queue_node_t* head;
	/// Pointer to the last element in the queue
	queue_node_t* tail;
} queue_t;

/// Create an empty queue
queue_t* queue_create();

/// Appends an element to the queue
/// @return A pointer to the data on success, NULL on error
void* queue_append(queue_t* queue, void* data);

/// Returns a pointer to the data in the first element of the queue
/// The element is not removed from the queue
void* queue_peek(queue_t* queue);

/// Return the number of elements in the queue
size_t queue_count(const queue_t* queue);

/// Return true if the given queue is empty
bool queue_is_empty(const queue_t* queue);

/// Clear all elements from a queue
/// @param remove_data true if free() must be called for the data in each element
void queue_clear(queue_t* queue, bool remove_data);

/// Destroy a queue
void queue_destroy(queue_t* queue, bool remove_data);

/// Extracts the first element from the queue and returns its data
void* queue_pop(queue_t* queue);

#endif // QUEUE_H
