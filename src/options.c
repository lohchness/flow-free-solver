#include "utils.h"
#include "options.h"

// Global options struct gets setup during main
options_t g_options;

//////////////////////////////////////////////////////////////////////
// Command line usage

void usage(FILE* fp, int exitcode) {

	fprintf(fp,
		"usage: flow_solver [ OPTIONS ] BOARD1.txt\n"		
		"BOARD2.txt [ ... ] ]\n\n"
		"Display options:\n\n"
		"  -q, --quiet             Reduce output\n"
		"  -d, --diagnostics       Print diagnostics when search unsuccessful\n"
		"  -A, --animation         Animate solution\n"
		"  -F, --fast              Speed up animation 4x\n"
#ifndef _WIN32          
		"  -C, --color             Force use of ANSI color\n"
#endif
		"  -S, --svg               Output final state to SVG\n"
		"\n"
		"Node evaluation options:\n\n"
		"  -d, --deadends          dead-end checking\n"
		"\n"
		"Color ordering options:\n\n"
		"  -r, --randomize         Shuffle order of colors before solving\n"
		"  -c, --constrained       Disable order by most constrained\n"
		"\n"
		"Search options:\n\n"
		"  -n, --max-nodes N       Restrict storage to N nodes\n"
		"  -m, --max-storage N     Restrict storage to N MB (default %'g)\n"
		"\n"
		"Help:\n\n"
		"  -h, --help              See this help text\n\n",
		g_options.search_max_mb);

	exit(exitcode);
  
}

//////////////////////////////////////////////////////////////////////
//

const char* get_argument(int argc, char** argv, int* i) {

	assert(*i < argc);
  
	if ((*i)+1 == argc) {
		fprintf(stderr, "%s needs argument\n", argv[*i]);
		usage(stderr, 1);
	}

	return argv[++(*i)];
  
  
}

//////////////////////////////////////////////////////////////////////
// Check file exists

int exists(const char* fn) {

	FILE* fp = fopen(fn, "r");
  
	if (fp) {
		fclose(fp);
		return 1;
	} else {
		return 0;
	}

}


//////////////////////////////////////////////////////////////////////
// Parse command-line options

size_t parse_options(int argc, char** argv,
                     const char** input_files) {
  
	size_t num_inputs = 0;

	if (argc < 2) {
		fprintf(stderr, "not enough args!\n\n");
		usage(stderr, 1);
	}

	typedef struct flag_options_struct {
		int short_char;
		const char* long_string;
		int* dst_flag;
		int dst_value;
	} flag_options_t;

	flag_options_t options[] = {
		{ 'q', "quiet",         &g_options.display_quiet, 1 },
		{ 'i', "diagnostics",   &g_options.display_diagnostics, 1 },
		{ 'A', "animation",     &g_options.display_animate, 1 },
#ifndef _WIN32    
		{ 'C', "color",         &g_options.display_color, 1 },
#endif
		{ 'F', "fast",          &g_options.display_fast, 1 },
		{ 'S', "svg",           &g_options.display_save_svg, 1 },
		{ 'd', "deadends",      &g_options.node_check_deadends, 1 },
		{ 'r', "randomize",     &g_options.order_random, 1 },
		{ 'c', "constrained",   &g_options.order_most_constrained, 0 },
		{ 'n', "max-nodes",     0, 0 },
		{ 'm', "max-storage",   0, 0 },
		{ 'h', "help",          0, 0 },
		{ 0, 0, 0, 0 }
	};

	for (int i=1; i<argc; ++i) {
    
		const char* opt = argv[i];
		int match_id = -1;

		for (int k=0; options[k].short_char; ++k) {

			if (options[k].short_char > 0) {
				char cur_short[3] = "-?";
				cur_short[1] = options[k].short_char;
				if (!strcmp(opt, cur_short)) {
					match_id = k;
					break;
				}
			}

			if (options[k].long_string) {
				char cur_long[1024];
				snprintf(cur_long, 1024, "--%s", options[k].long_string);
				if (!strcmp(opt, cur_long)) {
					match_id = k;
					break;
				}
			}

		}

		if (match_id >= 0) {

			int match_short_char = options[match_id].short_char;

			if (options[match_id].dst_flag) {
        
				*options[match_id].dst_flag = options[match_id].dst_value;

			} else if (match_short_char == 'n') {

				opt = get_argument(argc, argv, &i);
      
				char* endptr;
				g_options.search_max_nodes = strtol(opt, &endptr, 10);
      
				if (!endptr || *endptr) {
					fprintf(stderr, "error parsing max nodes %s "
						"on command line!\n\n", opt);
					exit(1);
				}

			} else if (match_short_char == 'm') {

				opt = get_argument(argc, argv, &i);
        
				char* endptr;
				g_options.search_max_mb = strtod(opt, &endptr);
        
				if (!endptr || *endptr || g_options.search_max_mb <= 0) {
					fprintf(stderr, "error parsing max storage %s "
						"on command line!\n\n", opt);
					exit(1);
				}
        
			} else if (match_short_char == 'h') {

				usage(stdout, 0);

			} else { // should not happen

				fprintf(stderr, "unrecognized option: %s\n\n", opt);
				usage(stderr, 1);

			}

		} else if (exists(opt)) {

			input_files[num_inputs++] = opt;

		} else {

			fprintf(stderr, "unrecognized option: %s\n\n", opt);
			usage(stderr, 1);
      
		}
    
	}

	if (!num_inputs) {
		fprintf(stderr, "no input files\n\n");
		exit(1);
	}

	return num_inputs;

}
