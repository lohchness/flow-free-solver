#ifndef __OPTIONS__
#define __OPTIONS__

#include <stdio.h>
#include <stdint.h>

// Options for this program
typedef struct options_struct {

	int    display_quiet;
	int    display_diagnostics;
	int    display_animate;
	int    display_color;
	int    display_fast;
	int    display_save_svg;  

	int    node_check_deadends;
  
	int    order_most_constrained;
	int    order_random;

	size_t search_max_nodes;
	double search_max_mb;
  
} options_t;

//Parse Command-line options
size_t parse_options(int argc, char** argv, const char** input_files);

// Global options struct gets setup during main
extern options_t g_options;

#endif
