#ifndef __NODE__
#define __NODE__

#include <stdio.h>
#include <stdint.h>

#include "utils.h"
#include "engine.h"


/**
 * Data structure containing the node information
 */


// Search node for DIJKSTRA
typedef struct tree_node_struct {
	game_state_t state;              // Current game state
	double cost_to_node;             // Cost to node
	struct tree_node_struct* parent; // Parent of this node (may be NULL)
} tree_node_t;

// Compare total cost for nodes, used by heap functions below.
int node_compare(const tree_node_t* a, const tree_node_t* b);

// Create Node and update cost
tree_node_t* node_create(tree_node_t* parent, const game_info_t* info, const game_state_t* state);



//////////////////////////////////////////////////////////////////////
// Perform diagnostics on the given node

void node_diagnostics(const game_info_t* info, const tree_node_t* node);


//////////////////////////////////////////////////////////////////////
// Animate the solution by printing out boards in reverse order,
// following parent pointers back from solution to root.

void animate_solution(const game_info_t* info, const tree_node_t* node);



#endif
