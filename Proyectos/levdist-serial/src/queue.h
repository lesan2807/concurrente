#ifndef QUEUE_H
#define QUEUE_H

/** @file queue.h

Implements a forward linked queue that stores pointers as its values.
Pointers are internally managed as `void*`, so user of the queue can
store anything that can be addressed in the main memory.

The following example shows a way to store words from stdin in a queue:

```
int main()
{
	// Create a queue to store words
	queue_t* queue = queue_create();

	// Read words from stdin and duplicate them into the queue
	char word[1024];
	while ( scanf("%1023s", word) == 1 )
		queue_append(queue, strdup(word));

	// Print words one per line, using iterators
	for ( queue_iterator_t itr = queue_begin(queue); itr != queue_end(queue); itr = queue_next(itr) )
		printf("%s\n", (const char*)queue_data(itr));

	// Destroy the queue and its duplicated words
	queue_destroy(this->files, true);
	return 0;
}
```
*/

#include <stdbool.h>
#include <stddef.h>

/**
An opaque record that represents a node of a simply-linked list.
*/
typedef struct queue_node queue_node_t;

/**
An opaque record that contains attributes for managing a queue of pointers.
*/
typedef struct queue queue_t;

/**
	An opaque iterator for traversing a queue of pointers.
*/
typedef queue_node_t* queue_iterator_t;

/**
	@brief Create an empty queue.

	@return Pointer to the new created queue in heap memory.
	@remarks The pointed queue must be freed with @a queue_destroy().
 */
queue_t* queue_create();

/**
	@brief Return true if the given queue is empty.

	@param queue Pointer to the queue to be consulted.
	@return true if the queue is empty, false if it has elements.
*/
bool queue_is_empty(const queue_t* queue);

/**
	@brief Return the number of elements stored in the queue.

	@param queue Pointer to the queue to be consulted.
	@return The number of elements, 0 if the queue is empty.
	@remarks The que must not be empty.
*/
size_t queue_count(const queue_t* queue);

/**
	@brief Appends an element to the queue.

	@param queue Queue where the data will be appended.
	@param data The data to be added to the queue.
	@return A pointer to the data on success, NULL on error.
*/
void* queue_append(queue_t* queue, void* data);

/**
	@brief Get access to the data in the head of the queue.

	@param queue Queue where the data will be obtained.
	@return A pointer to the data in the first element of the queue.
	@remarks The element is not removed from the queue.
*/
void* queue_peek(queue_t* queue);

/**
	@brief Extracts the first element from the queue and returns its data.

	@details After this call, the queue decreases an element.
	@param queue Pointer to the queue where the first element will be extracted.
	@return Pointer to the data stored in the first element of the queue.
	@remarks The que must not be empty.
*/
void* queue_pop(queue_t* queue);

/**
	@brief Clear all elements from a queue.

	@details Only the elements in the queue will be removed. The queue itself
	becomes empty, so it continue being usable, for example, for adding more
	elements after a call to this function.
	@param queue Pointer to the queue to be cleared.
	@param remove_data true if free() must be called for the data in each element.
*/
void queue_clear(queue_t* queue, bool remove_data);

/**
	@brief Releases the memory occupied by the queue.

	@details Both, the elements and the queue are removed from memory. Any
	pointer to the queue becomes invalid after a call to this function.
	@param queue Pointer to the queue to be destroyed.
	@param remove_data If false only the memory occupied by the queue and its
	data structures (nodes) will be released. If true, the pointed data in each
	node will be released as well. That is, the free() function will be called
	with each `void*` pointer stored in the queue.
*/
void queue_destroy(queue_t* queue, bool remove_data);

/**
	@brief Returns an iterator to the first element of the queue

	@param queue Pointer to the queue to be traversed.
	@return A valid iterator to the first element if the queue is not empty,
	an invalid iterator to the end of the queue if it is empty.
*/
queue_iterator_t queue_begin(queue_t* queue);

/**
	@brief Returns an iterator that represents an invalid element.

	@param queue Pointer to the queue to be traversed.
	@return An invalid iterator that indicates the end of the queue.
	@remarks Caller must not try to get the pointed data through this iterator.
*/
queue_iterator_t queue_end(queue_t* queue);

/**
	@brief Returns an iterator to the next element.

	@param iterator A current iterator.
	@return A valid iterator if @a iterator has a next element, otherwise
	an invalid iterator that indicates the end of the queue.
	@remarks Param @a iterator must be valid.
*/
queue_iterator_t queue_next(queue_iterator_t iterator);

/**
	@brief Returns the data pointed by the given iterator.

	@param iterator An iterator poting to some data in the queue.
	@return A pointer to the data stored in the element being pointed by the
	@a iterator param.
	@remarks Param @a iterator must be valid
*/
void* queue_data(queue_iterator_t iterator);


#endif // QUEUE_H
