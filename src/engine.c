#include "engine.h"
#include "utils.h"
#include "options.h"

//////////////////////////////////////////////////////////////////////
// Print out game board

void game_print(const game_info_t* info,
                const game_state_t* state) {

	printf("%s", BLOCK_CHAR);
	for (size_t x=0; x<info->size; ++x) {
		printf("%s", BLOCK_CHAR);
	}
	printf("%s\n", BLOCK_CHAR);

	for (size_t y=0; y<info->size; ++y) {
		printf("%s", BLOCK_CHAR);
		for (size_t x=0; x<info->size; ++x) {
			cell_t cell = state->cells[pos_from_coords(x, y)];
			printf("%s", color_cell_str(info, cell));
		}
		printf("%s\n", BLOCK_CHAR);
	}

	printf("%s", BLOCK_CHAR);
	for (size_t x=0; x<info->size; ++x) {
		printf("%s", BLOCK_CHAR);
	}
	printf("%s\n", BLOCK_CHAR);
  
}


//////////////////////////////////////////////////////////////////////
// Consider whether the given move is valid.

int game_can_move(const game_info_t* info,
                  const game_state_t* state,
                  int color, int dir) {

	// Make sure color is valid
	assert(color < info->num_colors);

	assert(!(state->completed & (1 << color)));

	// Get cur pos x, y
	int cur_x, cur_y;
	pos_get_coords(state->pos[color], &cur_x, &cur_y);

	// Get new x, y
	int new_x = cur_x + DIR_DELTA[dir][0];
	int new_y = cur_y + DIR_DELTA[dir][1];

	// If outside bounds, not legal
	if (new_x < 0 || new_x >= info->size ||
	    new_y < 0 || new_y >= info->size) {
		return 0;
	}

	// Create a new position
	pos_t new_pos = pos_from_coords(new_x, new_y);
	assert( new_pos < MAX_CELLS );

       
	// Must be empty (TYPE_FREE)
	if (state->cells[new_pos]) {
		return 0;
	}

	
    
	// All puzzles are designed so that a new path segment is adjacent
	// to at most one path segment of the same color -- the predecessor
	// to the new segment. We check this by iterating over the
	// neighbors.
	for (int dir=0; dir<4; ++dir) {

		// Assemble position
		pos_t neighbor_pos = offset_pos(info, new_x, new_y, dir);

		// If valid non-empty cell and not cur_pos and not goal_pos and
		// has our color, then fail
		if ( neighbor_pos != INVALID_POS && 
		     state->cells[neighbor_pos] &&
		     neighbor_pos != state->pos[color] && 
		     neighbor_pos != info->goal_pos[color] && 
		     cell_get_color(state->cells[neighbor_pos]) == color) {
			return 0;
		}
    
	}

	
  
	// It's valid
	return 1;

}



//////////////////////////////////////////////////////////////////////
// Update the game state to make the given move.

void game_make_move(const game_info_t* info,
                      game_state_t* state, 
                      int color, int dir) {


	// Make sure valid color
	assert(color < info->num_colors);

	// Update the cell with the new cell value
	cell_t move = cell_create(TYPE_PATH, color, dir);
  
	// Get current x, y
	int cur_x, cur_y;
	pos_get_coords(state->pos[color], &cur_x, &cur_y);

	// Assemble new x, y
	int new_x = cur_x + DIR_DELTA[dir][0];
	int new_y = cur_y + DIR_DELTA[dir][1];

	// Make sure valid
	assert( new_x >= 0 && new_x < (int)info->size &&
		new_y >= 0 && new_y < (int)info->size );

	// Make position
	pos_t new_pos = pos_from_coords(new_x, new_y);
	assert( new_pos < MAX_CELLS );

	// Make sure it's empty
	assert( state->cells[new_pos] == 0 );

	// Update cells and new pos
	state->cells[new_pos] = move;
	state->pos[color] = new_pos;
	--state->num_free;

	state->last_color = color;

	int goal_dir = -1;


	for (int dir=0; dir<4; ++dir) {
		if (offset_pos(info, new_x, new_y, dir) == info->goal_pos[color]) {
			goal_dir = dir;
			break;
		}
	}
	

	if (goal_dir >= 0) {

		state->cells[info->goal_pos[color]] = cell_create(TYPE_GOAL, color, goal_dir);
		state->completed |= (1 << color);    
	} 

}


//////////////////////////////////////////////////////////////////////
// Pick the next color to move deterministically

int game_next_move_color(const game_info_t* info,
                         const game_state_t* state) {


	size_t last_color = state->last_color;

	if (last_color < info->num_colors &&
	    !(state->completed & (1 << last_color))) {
		return last_color;
	}

	// return the color with less number of free cells
	// Do not return a color which is already completed!
	if ( g_options.order_most_constrained) {

		size_t best_color = -1;
		int best_free = 4;

		
		for (size_t i=0; i<info->num_colors; ++i) {

			int color = info->color_order[i];
      
			if (state->completed & (1 << color)) {
				continue;
			}
      
			int num_free = game_num_free_pos(info, state,
							 state->pos[color]);

			if (num_free < best_free) {
				best_free = num_free;
				best_color = color;
			}
		
		}

		
		assert(best_color < info->num_colors);
		return best_color;
    
	} else {

		for (size_t i=0; i<info->num_colors; ++i) {
			int color = info->color_order[i];
			if (state->completed & (1 << color)) { continue; }
			return color;
		}

		assert(0 && "unreachable code");
		return -1;
    
	} 

}

//////////////////////////////////////////////////////////////////////
// Return the number of free spaces around an x, y position

int game_num_free_coords(const game_info_t* info,
                         const game_state_t* state,
                         int x, int y) {

	int num_free = 0;
  
	for (int dir=0; dir<4; ++dir) {
		pos_t neighbor_pos = offset_pos(info, x, y, dir);
		if (neighbor_pos != INVALID_POS &&
		    state->cells[neighbor_pos] == 0) {
			++num_free;
		}
	}

	return num_free;

}

//////////////////////////////////////////////////////////////////////
// Return the number of free spaces around an 8-bit position

int game_num_free_pos(const game_info_t* info,
                      const game_state_t* state,
                      pos_t pos) {

	int x, y;

	pos_get_coords(pos, &x, &y);
	return game_num_free_coords(info, state, x, y);

}

//////////////////////////////////////////////////////////////////////
// Return free if in bounds and unoccupied

int game_is_free(const game_info_t* info,
                 const game_state_t* state,
                 int x, int y) {

	return (coords_valid(info, x, y) &&
		state->cells[pos_from_coords(x, y)] == 0);
  
}




//////////////////////////////////////////////////////////////////////
// Helper function for below.

int detect_format(FILE* fp) {

	int max_letter = 'A';
	int c;

	while ((c = fgetc(fp)) != EOF) {
		if (isalpha(c) && c > max_letter) {
			max_letter = c;
		}
	}

	rewind(fp);

	return (max_letter - 'A') < MAX_COLORS;

}

//////////////////////////////////////////////////////////////////////
// Read game board from text file

int game_read(const char* filename,
              game_info_t* info,
              game_state_t* state) {

	FILE* fp = fopen(filename, "r");

	if (!fp) {
		fprintf(stderr, "error opening %s\n", filename);
		return 0;
	}

	int is_alternate_format = detect_format(fp);

	memset(info, 0, sizeof(game_info_t));
	memset(state, 0, sizeof(game_state_t));
  
	memset(state->pos, 0xff, sizeof(state->pos));

	state->last_color = MAX_COLORS;

	size_t y=0;

	char buf[MAX_SIZE+2];

	memset(info->color_tbl, 0xff, sizeof(info->color_tbl));
	memset(info->init_pos, 0xff, sizeof(info->init_pos));
	memset(info->goal_pos, 0xff, sizeof(info->goal_pos));

	while (info->size == 0 || y < info->size) {

		char* s = fgets(buf, MAX_SIZE+1, fp);
		size_t l = s ? strlen(s) : 0;
    
		if (!s) {
			fprintf(stderr, "%s:%zu: unexpected EOF\n", filename, y+1);
			fclose(fp);
			return 0;
		} else if (s[l-1] != '\n') {
			fprintf(stderr, "%s:%zu line too long\n", filename, y+1);
			fclose(fp);
			return 0;
		}

		if (l >= 2 && s[l-2] == '\r') { // DOS line endings
			--l;
		}

		if (info->size == 0) {
			if (l < 3) {
				fprintf(stderr, "%s:1: expected at least 3 characters before newline\n",
					filename);
				fclose(fp);
				return 0;
			} else if (l-1 > MAX_SIZE) {
				fprintf(stderr, "%s:1: size too big!\n", filename);
				fclose(fp);
				return 0;
			}
			info->size = l-1;
		} else if (l != info->size + 1) {
			fprintf(stderr, "%s:%zu: wrong number of characters before newline "
				"(expected %zu, but got %zu)\n",
				filename, y+1,
				info->size, l-1);
			fclose(fp);
			return 0;
		}

		for (size_t x=0; x<info->size; ++x) {
      
			uint8_t c = s[x];
      
			if (isalpha(c)) {

				pos_t pos = pos_from_coords(x, y);
				assert(pos < MAX_CELLS);

				int color = info->color_tbl[c];
        
				if (color >= info->num_colors) {

					color = info->num_colors;
          
					if (info->num_colors == MAX_COLORS) {
						fprintf(stderr, "%s:%zu: can't use color %c"
							"- too many colors!\n",
							filename, y+1, c);
						fclose(fp);
						return 0;

					}
          
					int id = is_alternate_format ? (c - 'A') : get_color_id(c);
					if (id < 0 || id >= MAX_COLORS) {
						fprintf(stderr, "%s:%zu: unrecognized color %c\n",
							filename, y+1, c);
						fclose(fp);
						return 0;
					}

					info->color_ids[color] = id;
					info->color_order[color] = color;
          
					++info->num_colors;
					info->color_tbl[c] = color;
					info->init_pos[color] = state->pos[color] = pos;
					state->cells[pos] = cell_create(TYPE_INIT, color, 0);

				} else {

					if (info->goal_pos[color] != INVALID_POS) {
						fprintf(stderr, "%s:%zu too many %c already!\n",
							filename, y+1, c);
						fclose(fp);
						return 0;
					}
					info->goal_pos[color] = pos;
					state->cells[pos] = cell_create(TYPE_GOAL, color, 0);

				}
        
			} else {

				++state->num_free;

			}
		}
    
		++y;
	}

	fclose(fp);

	if (!info->num_colors) {
		fprintf(stderr, "empty map!\n");
		return 0;
	}

	for (size_t color=0; color<info->num_colors; ++color) {

		if (info->goal_pos[color] == INVALID_POS) {
			game_print(info, state);
			fprintf(stderr, "\n\n%s: color %s has start but no end\n",
				filename,
				color_name_str(info, color));
			return 0;
		}

		
		//Makes sure the initial position is closer to the wall,
		//and the goal position is further from the wall.
		//This is not neccessary, but deterministically sets where to start the path
		int init_dist = pos_get_wall_dist(info, info->init_pos[color]);
		int goal_dist = pos_get_wall_dist(info, info->goal_pos[color]);
		
		if (goal_dist < init_dist) {
			pos_t tmp_pos = info->init_pos[color];
			info->init_pos[color] = info->goal_pos[color];
			info->goal_pos[color] = tmp_pos;
				state->cells[info->init_pos[color]] = cell_create(TYPE_INIT, color, 0);
				state->cells[info->goal_pos[color]] = cell_create(TYPE_GOAL, color, 0);
				state->pos[color] = info->init_pos[color];
		}

		

	}
  
	return 1;

}

//////////////////////////////////////////////////////////////////////
// Print out game board as SVG

void game_print_svg(FILE* fp,
                    const game_info_t* info,
                    const game_state_t* state) {

	size_t display_size = 256;
	size_t m = 1;

	size_t cell_size = (display_size - m * (info->size + 1)) / info->size;
	int xy_skip = cell_size + m;

	double dot_radius = cell_size * 0.35;
	double path_radius = cell_size * 0.35;

	display_size = xy_skip * info->size + m;

	fprintf(fp, "<svg xmlns=\"http://www.w3.org/2000/svg\" "
		"width=\"%zu\" height=\"%zu\">\n",
		display_size, display_size);

	fprintf(fp, "  <rect width=\"%zu\" height=\"%zu\" "
		"style=\"fill: #7b7c41;\" />\n",
		display_size, display_size);

	for (size_t y=0; y<info->size; ++y) {
    
		size_t display_y = m+xy_skip*y;
    
		for (size_t x=0; x<info->size; ++x) {
      
			size_t display_x = m+xy_skip*x;

			pos_t pos = pos_from_coords(x,y);
			cell_t cell = state->cells[pos];
			int color = cell_get_color(cell);
			int type  = cell_get_type(cell);

			const char* cell_bg = "000000";
        
			if (cell) {

				if (type == TYPE_PATH ||
				    (type == TYPE_INIT) ||
				    (type == TYPE_GOAL && (state->completed & (1 << color)))) {
					cell_bg = color_dict[info->color_ids[color]].bg_rgb;
				} 
        
			}

			fprintf(fp, "  <rect x=\"%zu\" y=\"%zu\" "
				"width=\"%zu\" height=\"%zu\" "
				"style=\"fill: #%s;\" />\n",
				display_x, display_y, cell_size, cell_size, cell_bg);

			if (type == TYPE_INIT || type == TYPE_GOAL) {

				double center_x = display_x + 0.5*cell_size;
				double center_y = display_y + 0.5*cell_size;

				fprintf(fp, "  <circle cx=\"%g\" cy=\"%g\" "
					"r=\"%g\" style=\"fill: #%s;\" />\n",
					center_x, center_y, dot_radius,
					color_dict[info->color_ids[color]].fg_rgb);


			}

		}
	}

	for (int color=0; color<info->num_colors; ++color) {

		pos_t pos = (state->completed & (1 << color)) ?
			info->goal_pos[color] : state->pos[color];

		if (pos == info->init_pos[color]) { continue; }

		int x, y;
		pos_get_coords(pos, &x, &y);
    
		double px = m + xy_skip*x + 0.5*cell_size;
		double py = m + xy_skip*y + 0.5*cell_size;

		fprintf(fp, "  <path d=\"M %g,%g ", px, py);

		while (1) {

			cell_t cell = state->cells[pos];
			assert( cell_get_color(cell) == color );
      
			int dir = cell_get_direction(cell);
			dir ^= 1; // flip direction

			if (dir == DIR_LEFT || dir == DIR_RIGHT) {
				fprintf(fp, "h %d ", dir == DIR_LEFT ? -xy_skip : xy_skip);
			} else {
				fprintf(fp, "v %d ", dir == DIR_UP ? -xy_skip : xy_skip);
			}
      
			int npos = pos_offset_pos(info, pos, dir);
			if (npos == INVALID_POS) { break; }

			pos = npos;

			if (pos == info->init_pos[color]) {
				break;
			}

		}

		fprintf(fp, " \" style=\"stroke: #%s; stroke-width: %g; "
			"fill: none; stroke-linecap: round\" />\n",
			color_dict[info->color_ids[color]].fg_rgb,
			path_radius);
    
	}
  
	fprintf(fp, "</svg>\n");

}

//////////////////////////////////////////////////////////////////////
// Thin wrapper on above.

void game_save_svg(const char* filename,
                   const game_info_t* info,
                   const game_state_t* state) {

	FILE* fp = fopen(filename, "w");
	if (fp) {
		game_print_svg(fp, info, state);
		fclose(fp);
	}

}

