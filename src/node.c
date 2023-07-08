#include "node.h"
#include "engine.h"
#include "extensions.h"

//////////////////////////////////////////////////////////////////////
// Compare total cost for nodes, used by heap functions below.

int node_compare(const tree_node_t* a,
                 const tree_node_t* b) {

	double af = a->cost_to_node;
	double bf = b->cost_to_node;

	if (af != bf) {
		return af < bf ? -1 : 1;
	} else {
		return a < b ? -1 : 1;
	}

}

//////////////////////////////////////////////////////////////////////
// Create a node and set the cost to the node

tree_node_t* node_create(tree_node_t* parent,
                         const game_info_t* info,
                         const game_state_t* state) {
  
	tree_node_t* new_n = malloc(sizeof(tree_node_t));
	if (!new_n) { return 0; }

	new_n->parent = parent;
	new_n->cost_to_node = 0;

	// update cost to node, unless is the root
	const size_t action_cost = 1;
	if (parent)
		new_n->cost_to_node = parent->cost_to_node + action_cost;
	
	memcpy(&(new_n->state), state, sizeof(game_state_t));
  
	return new_n;

}

//////////////////////////////////////////////////////////////////////
// Perform diagnostics on the given node

void node_diagnostics(const game_info_t* info,
		      const tree_node_t* node) {

	printf("\n###################################"
	       "###################################\n\n");

	printf("node has cost to node %'g and cost to go %'d\n",
	       node->cost_to_node, node->state.num_free);

	if (node->state.last_color < info->num_colors) {
		printf("last move was for color %s\n",
		       color_name_str(info, node->state.last_color));

	} else {
		printf("no moves yet?\n");
	}


	
  
}


//////////////////////////////////////////////////////////////////////
// Animate the solution by printing out boards in reverse order,
// following parent pointers back from solution to root.

void animate_solution(const game_info_t* info,
                           const tree_node_t* node) {

	if (node->parent) {
		animate_solution(info, node->parent);
	}

	printf("%s", unprint_board(info));
	game_print(info, &node->state);
	fflush(stdout);

	delay_seconds(0.1);
  
}
