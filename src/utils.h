#ifndef __UTILS__
#define __UTILS__

#include <stdio.h>
#include <stdint.h>
#include <locale.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include <math.h>
#include <time.h>

#include "engine.h"

////////////////////////////////////////////////////////
// Search termination results
enum {
	SEARCH_SUCCESS = 0,
	SEARCH_UNREACHABLE = 1,
	SEARCH_FULL = 2,
	SEARCH_IN_PROGRESS = 3,
};


//////////////////////////////////////////////////////
// Match color characters to ANSI color codes
typedef struct color_lookup_struct {
	char input_char;   // Color character
	char display_char; // Punctuation a la nethack
	const char* ansi_code;    // ANSI color code
	const char* fg_rgb;
	const char* bg_rgb;
} color_lookup_t;


////////////////////////////////////////////
// Used for auto-sorting colors
typedef struct color_features_struct {
	int index;
	int user_index;
	int wall_dist[2];
	int min_dist;
} color_features_t;

// Was gonna try some unicode magic but meh
extern const char* BLOCK_CHAR;

// For visualizing cardinal directions ordered by the enum above.
extern const char DIR_CHARS[4];

///////////////////////////////////////////////////////
// x, y, pos coordinates for each direction
extern const int DIR_DELTA[4][3];

// Look-up table mapping characters in puzzle definitions to 
// output char, ANSI color, foreground/background RGB
extern const color_lookup_t color_dict[MAX_COLORS];

// For succinct printing of search results
extern const char SEARCH_RESULT_CHARS[4];

// For verbose printing of search results
extern const char* SEARCH_RESULT_STRINGS[4];


//////////////////////////////////////////////////////////////////////
// Peform lookup in color_dict above

int get_color_id(char c);

//////////////////////////////////////////////////////////////////////
// Detect whether terminal supports color & cursor commands

int terminal_has_color();

//////////////////////////////////////////////////////////////////////
// Emit color string for index into color_dict table above

const char* color_char(const char* ansi_code, char color_out, char mono_out);

//////////////////////////////////////////////////////////////////////
// Clear screen and set cursor pos to 0,0

const char* unprint_board(const game_info_t* info);

//////////////////////////////////////////////////////////////////////
// Are the coords on the map?

int coords_valid(const game_info_t* info, int x, int y);

//////////////////////////////////////////////////////////////////////
// Compose an offset as a position and return whether valid or not

pos_t offset_pos(const game_info_t* info, int x, int y, int dir);

//////////////////////////////////////////////////////////////////////
// Compose an offset as a position and return whether valid or not

pos_t pos_offset_pos(const game_info_t* info, pos_t pos, int dir);

//////////////////////////////////////////////////////////////////////
// Get the distance from the wall for x, y coords

int get_wall_dist(const game_info_t* info, int x, int y);

//////////////////////////////////////////////////////////////////////
// Get the distance from the wall for 8-bit position

int pos_get_wall_dist(const game_info_t* info, pos_t pos);

//////////////////////////////////////////////////////////////////////
// Create a cell from a 2-bit type, a 4-bit color, and a 2-bit
// direction.

cell_t cell_create(uint8_t type, uint8_t color, uint8_t dir);

//////////////////////////////////////////////////////////////////////
// Get the type from a cell value

uint8_t cell_get_type(cell_t c);

//////////////////////////////////////////////////////////////////////
// Get the direction from a cell value

uint8_t cell_get_direction(cell_t c);

//////////////////////////////////////////////////////////////////////
// Get the color from a cell value

uint8_t cell_get_color(cell_t c);

//////////////////////////////////////////////////////////////////////
// For displaying a color nicely

const char* color_name_str(const game_info_t* info, int color);

//////////////////////////////////////////////////////////////////////
// For displaying a cell nicely

const char* color_cell_str(const game_info_t* info,cell_t cell);

//////////////////////////////////////////////////////////////////////
// Compare 2 ints

int cmp(int a, int b);

//////////////////////////////////////////////////////////////////////
// Return the current time as a double. Don't actually care what zero
// is cause we will just offset.

double now();

//////////////////////////////////////////////////////////////////////
// Create a delay

void delay_seconds(double s) ;

//////////////////////////////////////////////////////////////////////
// Create a 8-bit position from 2 4-bit x,y coordinates

pos_t pos_from_coords(pos_t x, pos_t y);

//////////////////////////////////////////////////////////////////////
// Split 8-bit position into 4-bit x & y coords

void pos_get_coords(pos_t p, int* x, int* y);

#endif
