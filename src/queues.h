#ifndef __QUEUES__
#define __QUEUES__

#include "options.h"
#include "node.h"

// Data structure for heap based priority queue
typedef struct heapq_struct {
	tree_node_t** start; // Array of node pointers
	size_t capacity;     // Maximum allowable queue size
	size_t count;        // Number enqueued
	size_t total_count;  // Total Number enqueued
} heapq_t;

// First in, first-out queue implemented as an array of pointers.
typedef struct queue_struct {
	tree_node_t** start; // Array of node pointers
	size_t capacity;     // Maximum number of things to enqueue ever
	size_t count;        // Total enqueued (next one will go into start[count])
	size_t next;         // Next index to dequeue
} queue_t;



//////////////////////////////////////////////////////////////////////
// Create a binary heap to store the given # of nodes

heapq_t heapq_create(size_t max_nodes);

//////////////////////////////////////////////////////////////////////
// Is heap queue count

size_t heapq_count(const heapq_t* q);

//////////////////////////////////////////////////////////////////////
// Is heap queue empty?

int heapq_empty(const heapq_t* q);

//////////////////////////////////////////////////////////////////////
// Peek at the next item to be removed

const tree_node_t* heapq_peek(const heapq_t* q);

//////////////////////////////////////////////////////////////////////
// Enqueue a node onto the heap

void heapq_enqueue(heapq_t* q, tree_node_t* node);

//////////////////////////////////////////////////////////////////////
// Pop a node off the heap

tree_node_t* heapq_deque(heapq_t* q);

//////////////////////////////////////////////////////////////////////
// Free memory allocated for heap

void heapq_destroy(heapq_t* q);



//////////////////////////////////////////////////////////////////////
// QUEUE via flat array

queue_t queue_create(size_t max_nodes);

//////////////////////////////////////////////////////////////////////
// Check if empty

int queue_empty(const queue_t* q);

//////////////////////////////////////////////////////////////////////
// Check count

size_t queue_count(const queue_t* q);

//////////////////////////////////////////////////////////////////////
// Push node into QUEUE

void queue_enqueue(queue_t* q, tree_node_t* n);

//////////////////////////////////////////////////////////////////////
// Peek at current QUEUE node

const tree_node_t* queue_peek(const queue_t* q);

//////////////////////////////////////////////////////////////////////
// Dequeue node from QUEUE

tree_node_t* queue_deque(queue_t* q);

//////////////////////////////////////////////////////////////////////
// De-allocate storage for QUEUE

void queue_destroy(queue_t* q);
#endif
