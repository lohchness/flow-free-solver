/**
 * This project has been adapted from
 * https://github.com/mzucker/flow_solver
 */

#include "utils.h"
#include "node.h"
#include "options.h"
#include "queues.h"
#include "engine.h"
#include "extensions.h"
#include "search.h"


//////////////////////////////////////////////////////////////////////
// Main function

int main(int argc, char** argv) {

	setlocale(LC_NUMERIC, "");

	g_options.display_quiet = 0;
	g_options.display_diagnostics = 0;
	g_options.display_animate = 0;
	g_options.display_color = terminal_has_color();
	g_options.display_fast = 0;
	g_options.display_save_svg = 0;
  
	g_options.node_check_deadends = 0;
	g_options.order_most_constrained = 1;

	g_options.search_max_nodes = 0;
	g_options.search_max_mb = 1024;

	const char* input_files[argc];

	memset(input_files, 0, sizeof(input_files));
  
	size_t num_inputs = parse_options(argc, argv,
					  input_files);

	game_info_t  info;
	game_state_t state;
  
	int max_width = 11;

	for (size_t i=0; i<num_inputs; ++i) {
		int l = strlen(input_files[i]);
		if (l > max_width) { max_width = l; }
	}

	int boards = 0;
	double total_elapsed[3] = { 0, 0, 0 };
	size_t total_nodes[3]   = { 0, 0, 0 };
	int    total_count[3]   = { 0, 0, 0 };
  
	for (size_t i=0; i<num_inputs; ++i) {

		const char* input_file = input_files[i];
  
		if (game_read(input_file, &info, &state)) {

			if (boards++ && !g_options.display_quiet) {
				printf("\n***********************************"
				       "***********************************\n\n");
			}

      
			if (!g_options.display_quiet) {
				printf("read %zux%zu board with %zu colors from %s\n",
				       info.size, info.size, info.num_colors, input_file);
				printf("\n");
			}

			game_order_colors(&info, &state);

			double elapsed;
			size_t nodes;
			game_state_t final_state = state;

			if (g_options.display_quiet) { 
				printf("%*s ", max_width, input_file);
				fflush(stdout);
			}


			int result = game_dijkstra_search(&info, &state, &elapsed, &nodes, &final_state);
			

			// If search is still in progress, then throw error
			//assert( result >= 0 && result < 3 );

			total_elapsed[result] += elapsed;
			total_nodes[result] += nodes;
			total_count[result] += 1;

			if (!g_options.display_quiet) {
  

				double q_mb = (nodes * (double)sizeof(tree_node_t) / MEGABYTE);

				printf("\nsearch %s after %'.3f seconds and %'zu nodes (%'.2f MB)\n",
				       SEARCH_RESULT_STRINGS[result],
				       elapsed,
				       nodes, q_mb);


			}
			else {
				printf("%c %'12.3f %'12zu\n",
				       SEARCH_RESULT_CHARS[result],
				       elapsed, nodes);
				
			}

			if (g_options.display_save_svg) {

				char output_file[1024];

				size_t start = 0;
				size_t end = strlen(input_file);
				for (size_t i=0; input_file[i]; ++i) {
					if (input_file[i] == '/') { start = i+1; }
					if (input_file[i] == '.' && i > start) { end = i; }
				}
				size_t l = end-start;
				if (l > 1019) { l = 1019; }
				strncpy(output_file, input_file+start, l);

				for (int i=0; i<5; ++i) {
					output_file[l++] = ".svg"[i];
				}
        
				game_save_svg(output_file, &info, &final_state);
				if (!g_options.display_quiet) {
					printf("wrote %s\n", output_file);
				}
        
			}
      
		}

	}

	if (boards > 1) {

		double overall_elapsed = 0;
		size_t overall_nodes = 0;
		int types = 0;
    
		for (int i=0; i<3; ++i) {
			overall_elapsed += total_elapsed[i];
			overall_nodes += total_nodes[i];
			if (total_nodes[i]) { ++types; }
		}

		if (!g_options.display_quiet) {

			printf("\n***********************************"
			       "***********************************\n\n");

			for (int i=0; i<3; ++i) {
				if (total_count[i]) {
					printf("%'d %s searches took a total of %'.3f seconds and %'zu nodes\n",
					       total_count[i], SEARCH_RESULT_STRINGS[i],
					       total_elapsed[i], total_nodes[i]);
				}
			}

			if (types > 1) {
				printf("\n");
				printf("overall, %'d searches took a total of %'.3f seconds "
				       "and %'zu nodes\n",
				       boards, overall_elapsed, overall_nodes);
			}
      
		} else {
      
			printf("\n");
			for (int i=0; i<3; ++i) {
				if (total_count[i]) {
					printf("%*s%3d total %c %'12.3f %'12zu\n",
					       max_width-9, "",
					       total_count[i],
					       SEARCH_RESULT_CHARS[i],
					       total_elapsed[i],
					       total_nodes[i]);
				}
			}

			if (types > 1) {
				printf("\n");
				printf("%*s%3d overall %'12.3f %'12zu\n",
				       max_width-9, "",
				       boards,
				       overall_elapsed,
				       overall_nodes);
			}
      
		}
    
	}  
    
	return 0;
  
}
