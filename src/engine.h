#ifndef __ENGINE__
#define __ENGINE__

#include <stdio.h>
#include <stdint.h>
#include <locale.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include <math.h>
#include <time.h>

///////////////////////////////////////////////////////////
// Positions are 8-bit integers with 4 bits each for y, x.
enum {

	// Number to represent "not found"
	INVALID_POS = 0xff,
  
	// Maximum # of colors in a puzzle
	MAX_COLORS = 16,
  
	// Maximum valid size of a puzzle
	MAX_SIZE = 15,
  
	// Maximum # cells in a valid puzzle -- since we just use bit
	// shifting to do x/y, need to allocate space for 1 unused column.
	MAX_CELLS = (MAX_SIZE+1)*MAX_SIZE-1,
  
	// One million(ish) bytes
	MEGABYTE = 1024*1024,
  
};

////////////////////////////////////////////////////////
// Represent the contents of a cell on the game board
typedef uint8_t cell_t;

///////////////////////////////////////////////////////
// Represent a position within the game board
typedef uint8_t pos_t;


///////////////////////////////////////////////////////////////////////
// Static information about a puzzle layout -- anything that does not
// change as the puzzle is solved is stored here.
typedef struct game_info_struct {

	// Index in color_dict table of codes
	int    color_ids[MAX_COLORS];

	// Color order
	int    color_order[MAX_COLORS];

	// Initial and goal positions
	pos_t  init_pos[MAX_COLORS];
	pos_t  goal_pos[MAX_COLORS];

	// Length/width of game board
	size_t size;

	// Number of colors present
	size_t num_colors;

	// Color table for looking up color ID
	uint8_t color_tbl[128];

  
} game_info_t;

////////////////////////////////////////////////////////////////////////
// Incremental game state structure for solving -- this is what gets
// written as the search progresses, one state per search node
typedef struct game_state_struct {

	// State of each cell in the world; a little wasteful to duplicate,
	// since only one changes on each move, but necessary for DIJKSSTRA
	// (would not be needed for depth-first search).
	cell_t   cells[MAX_CELLS];

	// Head position
	pos_t    pos[MAX_COLORS];

	// How many free cells?
	uint8_t  num_free;

	// Which was the last color / endpoint
	uint8_t  last_color;

	// Bitflag indicating whether each color has been completed or not
	// (cur_pos is adjacent to goal_pos).
	uint16_t completed;
  
} game_state_t;


///////////////////////////////////////////////////////
// Various cell types, all but freespace have a color
enum {
	TYPE_FREE = 0, // Free space
	TYPE_PATH = 1, // Path between init & goal
	TYPE_INIT = 2, // Starting point
	TYPE_GOAL = 3  // Goal position
};

/////////////////////////////////////////////////////////
// Enumerate cardinal directions so we can loop over them
// RIGHT is increasing x, DOWN is increasing y.
enum {
	DIR_LEFT  = 0,
	DIR_RIGHT = 1,
	DIR_UP    = 2,
	DIR_DOWN  = 3
};



//////////////////////////////////////////////////////////////////////
// Print out game board

void game_print(const game_info_t* info, const game_state_t* state);

//////////////////////////////////////////////////////////////////////
// Consider whether the given move is valid.

int game_can_move(const game_info_t* info, const game_state_t* state, int color, int dir);


//////////////////////////////////////////////////////////////////////
// Update the game state to make the given move.

void game_make_move(const game_info_t* info, game_state_t* state, int color, int dir);

//////////////////////////////////////////////////////////////////////
// Pick the next color to move deterministically

int game_next_move_color(const game_info_t* info, const game_state_t* state);


//////////////////////////////////////////////////////////////////////
// Return the number of free spaces around an x, y position

int game_num_free_coords(const game_info_t* info, const game_state_t* state, int x, int y);

//////////////////////////////////////////////////////////////////////
// Return the number of free spaces around an 8-bit position

int game_num_free_pos(const game_info_t* info, const game_state_t* state, pos_t pos);


//////////////////////////////////////////////////////////////////////
// Return free if in bounds and unoccupied

int game_is_free(const game_info_t* info, const game_state_t* state, int x, int y);



//////////////////////////////////////////////////////////////////////
// Read game board from text file

int game_read(const char* filename, game_info_t* info, game_state_t* state);

//////////////////////////////////////////////////////////////////////
// Print out game board as SVG

void game_save_svg(const char* filename, const game_info_t* info, const game_state_t* state);

#endif
