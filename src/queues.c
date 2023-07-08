#include "queues.h"

//////////////////////////////////////////////////////////////////////
// Create a binary heap to store the given # of nodes

heapq_t heapq_create(size_t max_nodes) {
	heapq_t heapq;
	heapq.start = malloc(sizeof(tree_node_t*) * max_nodes);
	if (!heapq.start) {
		fprintf(stderr, "out of memory creating heapq!\n");
		exit(1);
	}
	heapq.count = 0;
	heapq.total_count=0;
	heapq.capacity = max_nodes;
	return heapq;
}

//////////////////////////////////////////////////////////////////////
// Indexing macros for heaps

#define HEAPQ_PARENT_INDEX(i) (((i)-1)/2)
#define HEAPQ_LCHILD_INDEX(i) ((2*(i))+1)

//////////////////////////////////////////////////////////////////////
// For debugging, not used presently

int heapq_valid(const heapq_t* q) {
	for (size_t i=1; i<q->count; ++i) {
		const tree_node_t* tc = q->start[i];
		const tree_node_t* tp = q->start[HEAPQ_PARENT_INDEX(i)];
		if (node_compare(tp, tc) > 0) {
			return 0;
		}
	}
	return 1;
}


//////////////////////////////////////////////////////////////////////
// Is heap queue count

size_t heapq_count(const heapq_t* q) {
	return q->total_count;
}


//////////////////////////////////////////////////////////////////////
// Is heap queue empty?

int heapq_empty(const heapq_t* q) {
	return q->count == 0;
}

//////////////////////////////////////////////////////////////////////
// Peek at the next item to be removed

const tree_node_t* heapq_peek(const heapq_t* q) {
	assert(!heapq_empty(q));
	return q->start[0];
}

//////////////////////////////////////////////////////////////////////
// Enqueue a node onto the heap

void heapq_enqueue(heapq_t* q, tree_node_t* node) {

	assert(q->count < q->capacity);

	size_t i = q->count++;
	q->total_count++;
	size_t pi = HEAPQ_PARENT_INDEX(i);
  
	q->start[i] = node;
  
	while (i > 0 && node_compare(q->start[pi], q->start[i]) > 0) {
		tree_node_t* tmp = q->start[pi];
		q->start[pi] = q->start[i];
		q->start[i] = tmp;
		i = pi;
		pi = HEAPQ_PARENT_INDEX(i);
	}
                      
}

//////////////////////////////////////////////////////////////////////
// Helper function used for dequeueing

void _heapq_repair(heapq_t* q, size_t i) {

	size_t li = HEAPQ_LCHILD_INDEX(i);
	size_t ri = li + 1;
	size_t smallest = i;

	if (li < q->count &&
	    node_compare(q->start[i], q->start[li]) > 0) {
		smallest = li;
	}

	if (ri < q->count &&
	    node_compare(q->start[smallest], q->start[ri]) > 0) {
		smallest = ri;
	}

	if (smallest != i){
		tree_node_t* tmp = q->start[i];
		q->start[i] = q->start[smallest];
		q->start[smallest] = tmp;
		_heapq_repair(q, smallest);
	}    

}

//////////////////////////////////////////////////////////////////////
// Pop a node off the heap

tree_node_t* heapq_deque(heapq_t* q) {

	assert(!heapq_empty(q));

	tree_node_t* rval = q->start[0];
	--q->count;

	if (q->count) {
		q->start[0] = q->start[q->count];
		_heapq_repair(q, 0);
	}
  
	return rval;
  
}

//////////////////////////////////////////////////////////////////////
// Free memory allocated for heap

void heapq_destroy(heapq_t* q) {
	free(q->start);
}

//////////////////////////////////////////////////////////////////////
// QUEUE via flat array

queue_t queue_create(size_t max_nodes) {
	queue_t queue;
	queue.start = malloc(sizeof(tree_node_t*) * max_nodes);
	if (!queue.start) {
		fprintf(stderr, "out of memory creating queue!\n");
		exit(1);
	}
	queue.count = 0;
	queue.capacity = max_nodes;
	queue.next = 0;
	return queue;
}

//////////////////////////////////////////////////////////////////////
// Check if empty

int queue_empty(const queue_t* q) {
	return q->next == q->count;
}

//////////////////////////////////////////////////////////////////////
// Check count

size_t queue_count(const queue_t* q) {
	return q->count;
}

//////////////////////////////////////////////////////////////////////
// Push node into QUEUE

void queue_enqueue(queue_t* q, tree_node_t* n) {
	assert(q->count < q->capacity);
	q->start[q->count++] = n;
}

//////////////////////////////////////////////////////////////////////
// Peek at current QUEUE node

const tree_node_t* queue_peek(const queue_t* q) {
	assert(!queue_empty(q));
	return q->start[q->next];
}

//////////////////////////////////////////////////////////////////////
// Dequeue node from QUEUE

tree_node_t* queue_deque(queue_t* q) {
	assert(!queue_empty(q));
	return q->start[q->next++];
}

//////////////////////////////////////////////////////////////////////
// De-allocate storage for QUEUE

void queue_destroy(queue_t* q) {
	free(q->start);
}

