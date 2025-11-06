#ifndef __JDL_H__
#define __JDL_H__

#define DEAD 0
#define ALIVE 1
#define BLACK L"\u2b1b"      // ⬛
#define WHITE L"\u2b1c"      // ⬜

#include <stdlib.h>
#include <ncursesw/ncurses.h>

struct cell_s 
{
    char value;
    char next_value;
    unsigned int x;
    unsigned int y;
    struct cell_s *top;
    struct cell_s *bot;
    struct cell_s *left;
    struct cell_s *right;
    struct cell_s *top_left;
    struct cell_s *top_right;
    struct cell_s *bot_left;
    struct cell_s *bot_right;
};
typedef struct cell_s cell;


struct grid_s
{
    cell ***grille;
    unsigned int x;
    unsigned int y;
};
typedef struct grid_s grid;


cell *init_targeted_square(unsigned int x, unsigned int y);
grid *init_grid_default(void);
void square_free(cell *target);
void grid_free(grid *target);
void grid_print(grid *target, WINDOW *win);
void grid_edit_square(grid *target, unsigned int x, unsigned int y, char square);
char cell_get_nb_alive_neighbours(cell *target);
void grid_next(grid *target);
void randomize_grid(grid *target);



#endif