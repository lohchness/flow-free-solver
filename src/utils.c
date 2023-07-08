
#ifndef _WIN32
#include <unistd.h>
#include <sys/time.h>
#else
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif


#include "utils.h"
#include "options.h"

// Was gonna try some unicode magic but meh
const char* BLOCK_CHAR = "#";

// For visualizing cardinal directions ordered by the enum above.
const char DIR_CHARS[4] = "<>^v";

///////////////////////////////////////////////////////
// x, y, pos coordinates for each direction
const int DIR_DELTA[4][3] = {
	{ -1, 0, -1 },
	{  1, 0,  1 },
	{  0, -1, -16 },
	{  0, 1, 16 }
};

// Look-up table mapping characters in puzzle definitions to 
// output char, ANSI color, foreground/background RGB
const color_lookup_t color_dict[MAX_COLORS] = {
	{ 'R', 'o', "101", "ff0000", "723939" }, // red
	{ 'B', '+', "104", "0000ff", "393972" }, // blue
	{ 'Y', '@', "103", "eeee00", "6e6e39" }, // yellow
	{ 'G', '*',  "42", "008100", "395539" }, // green
	{ 'O', 'x',  "43", "ff8000", "725539" }, // orange
	{ 'C', '%', "106", "00ffff", "397272" }, // cyan
	{ 'M', '?', "105", "ff00ff", "723972" }, // magenta
	{ 'm', 'v',  "41", "a52a2a", "5f4242" }, // maroon
	{ 'P', '^',  "45", "800080", "553955" }, // purple
	{ 'A', '=', "100", "a6a6a6", "5f5e5f" }, // gray
	{ 'W', '~', "107", "ffffff", "727272" }, // white
	{ 'g', '-', "102", "00ff00", "397239" }, // bright green
	{ 'T', '$',  "47", "bdb76b", "646251" }, // tan
	{ 'b', '"',  "44", "00008b", "393958" }, // dark blue
	{ 'c', '&',  "46", "008180", "395555" }, // dark cyan
	{ 'p', '.',  "35", "ff1493", "72415a" }, // pink?
};



//////////////////////////////////////////////////////////////////////
// Peform lookup in color_dict above

int get_color_id(char c) {
	for (int i=0; i<MAX_COLORS; ++i) {
		if (color_dict[i].input_char == c) {
			return i;
		}
	}
	return -1;
}

// For succinct printing of search results
const char SEARCH_RESULT_CHARS[4] = "suf?";

// For verbose printing of search results
const char* SEARCH_RESULT_STRINGS[4] = {
	"successful",
	"unsolvable",
	"out of memory",
	"in progress"
};

//////////////////////////////////////////////////////////////////////
// Detect whether terminal supports color & cursor commands

int terminal_has_color() {

#ifdef _WIN32

	return 0;

#else
  
	if (!isatty(STDOUT_FILENO)) {
		return 0;
	} 

	char* term = getenv("TERM");
	if (!term) { return 0; }
  
	return strstr(term, "xterm") == term || strstr(term, "rxvt") == term;

#endif
  
}

//////////////////////////////////////////////////////////////////////
// Emit color string for index into color_dict table above

const char* color_char(const char* ansi_code, char color_out, char mono_out) {

	static char buf[256];
                       
	if (g_options.display_color) {
		snprintf(buf, 256, "\033[30;%sm%c\033[0m",
			 ansi_code, color_out);
	} else {
		snprintf(buf, 256, "%c", mono_out);
	}
  
	return buf;
  
}

//////////////////////////////////////////////////////////////////////
// Clear screen and set cursor pos to 0,0

const char* unprint_board(const game_info_t* info) {
	if (g_options.display_color) {
		static char buf[256];
		snprintf(buf, 256, "\033[%zuA\033[%zuD",
			 info->size+2, info->size+2);
		return buf;
	} else {
		return "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n";
	}
}

//////////////////////////////////////////////////////////////////////
// Are the coords on the map?

int coords_valid(const game_info_t* info,
                 int x, int y) {

	return (x >= 0 && x < (int)info->size &&
		y >= 0 && y < (int)info->size);

}

//////////////////////////////////////////////////////////////////////
// Compote an offset as a position and return whether valid or not

pos_t offset_pos(const game_info_t* info,
                 int x, int y, int dir) {

	int offset_x = x + DIR_DELTA[dir][0];
	int offset_y = y + DIR_DELTA[dir][1];

	return coords_valid(info, offset_x, offset_y) ?
		pos_from_coords(offset_x, offset_y) : INVALID_POS;
  
}

//////////////////////////////////////////////////////////////////////
// Compote an offset as a position and return whether valid or not

pos_t pos_offset_pos(const game_info_t* info,
                     pos_t pos, int dir) {

	int x, y;
	pos_get_coords(pos, &x, &y);
	return offset_pos(info, x, y, dir);

}

//////////////////////////////////////////////////////////////////////
// Get the distance from the wall for x, y coords

int get_wall_dist(const game_info_t* info,
                  int x, int y) {

	int p[2] = { x, y };
	int d[2];

	for (int i=0; i<2; ++i) {
		int d0 = p[i];
		int d1 = info->size - 1 - p[i];
		d[i] = d0 < d1 ? d0 : d1;
	}

	return d[0] < d[1] ? d[0] : d[1];

}

//////////////////////////////////////////////////////////////////////
// Get the distance from the wall for 8-bit position

int pos_get_wall_dist(const game_info_t* info,
                      pos_t pos) {

	int x, y;

	pos_get_coords(pos, &x, &y);
	return get_wall_dist(info, x, y);

}

//////////////////////////////////////////////////////////////////////
// Create a cell from a 2-bit type, a 4-bit color, and a 2-bit
// direction.

cell_t cell_create(uint8_t type, uint8_t color, uint8_t dir) {
	return ((color & 0xf) << 4) | ((dir & 0x3) << 2) | (type & 0x3);
}

//////////////////////////////////////////////////////////////////////
// Get the type from a cell value

uint8_t cell_get_type(cell_t c) {
	return c & 0x3;
}

//////////////////////////////////////////////////////////////////////
// Get the direction from a cell value

uint8_t cell_get_direction(cell_t c) {
	return (c >> 2) & 0x3;
}

//////////////////////////////////////////////////////////////////////
// Get the color from a cell value

uint8_t cell_get_color(cell_t c) {
	return (c >> 4) & 0xf;
}

//////////////////////////////////////////////////////////////////////
// For displaying a color nicely

const char* color_name_str(const game_info_t* info,
                           int color) {

	const color_lookup_t* l = &color_dict[info->color_ids[color]];
	return color_char(l->ansi_code, l->input_char, l->display_char);

}

//////////////////////////////////////////////////////////////////////
// For displaying a cell nicely

const char* color_cell_str(const game_info_t* info,
                           cell_t cell) {

	int type = cell_get_type(cell);
	int color = cell_get_color(cell);
	int dir = cell_get_direction(cell);
  
	const color_lookup_t* l = &color_dict[info->color_ids[color]];
  
	switch (type) {
	case TYPE_FREE:
		return " ";
		break;
	case TYPE_PATH:
		return color_char(l->ansi_code,
				  DIR_CHARS[dir],
				  l->display_char);
		break;
	default:
		return color_char(l->ansi_code,
				  (type == TYPE_INIT ? 'o' : 'O'),
				  l->display_char);
	}

}

//////////////////////////////////////////////////////////////////////
// Compare 2 ints

int cmp(int a, int b) {
	return a < b ? -1 : a > b ? 1 : 0;
}

//////////////////////////////////////////////////////////////////////
// Return the current time as a double. Don't actually care what zero
// is cause we will just offset.

double now() {
  
#ifdef _WIN32
	union {
		LONG_LONG ns100; /*time since 1 Jan 1601 in 100ns units */
		FILETIME ft;
	} now;
	GetSystemTimeAsFileTime (&now.ft);
	return (double)now.ns100 * 1e-7; // 100 nanoseconds = 0.1 microsecond
#else
	struct timeval tv;
	gettimeofday(&tv, 0);
	return (double)tv.tv_sec + (double)tv.tv_usec * 1e-6;
#endif

}

//////////////////////////////////////////////////////////////////////
// Create a delay

void delay_seconds(double s) {
	if (g_options.display_fast) { s /= 4.0; }
#ifdef _WIN32
	// TODO: find win32 equivalent of usleep?
#else
	usleep((size_t)(s * 1e6));
#endif
}

//////////////////////////////////////////////////////////////////////
// Create a 8-bit position from 2 4-bit x,y coordinates

pos_t pos_from_coords(pos_t x, pos_t y) {
	return ((y & 0xf) << 4) | (x & 0xf);
}

//////////////////////////////////////////////////////////////////////
// Split 8-bit position into 4-bit x & y coords

void pos_get_coords(pos_t p, int* x, int* y) {
	*x = p & 0xf;
	*y = (p >> 4) & 0xf;
}

