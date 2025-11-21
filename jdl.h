#ifndef __JDL_H__
#define __JDL_H__

#define DEAD 0
#define ALIVE 1
#define BLACK L"\u2b1b"      // ⬛
#define WHITE L"\u2b1c"      // ⬜

#include <stdlib.h>
#include <ncursesw/ncurses.h>


typedef struct chain_cells_s chain_cells;

typedef struct cell_s 
{
    char value;
    char next_value;
    char will_be_processed;
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
    chain_cells *chain;
} cell;


typedef struct grid_s
{
    cell ***grille;
    unsigned int x;
    unsigned int y;
    chain_cells *head;
} grid;

#include "chained_list.h"


grid *create_grid(void);
cell *init_targeted_square(unsigned int x, unsigned int y);
grid *init_grid_default(void);
void square_free(cell *target);
void grid_free(grid *target);
void grid_print(grid *target, WINDOW *win);
void grid_edit_square(cell *target, char square, grid *game);
char cell_get_nb_alive_neighbours(cell *target);
void grid_next(grid *head);
void randomize_grid(grid *target);
cell **manage_process_queue(chain_cells *queue_start, unsigned int *nb_items);



#endif