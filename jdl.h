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
    signed long x;
    signed long y;
    struct cell_s *top;
    struct cell_s *bot;
    struct cell_s *left;
    struct cell_s *right;
    chain_cells *chain;
} cell;


typedef struct game_s
{
    cell *top_left;
    cell *bot_right;
    chain_cells *head;
} game;

typedef enum direction_s
{
    TOP,
    BOT,
    LEFT,
    RIGHT
} direction;

#include "chained_list.h"


game *create_game(void);
cell *init_targeted_cell(signed long y, signed long x);
void create_line(cell *left, signed long from_x, signed long until_x);
game *init_game_default(void);
void cell_free(cell *target);
void game_free(game *target);
void game_print(game *target, WINDOW *win);
void game_edit_square(cell *target, char square, game *game);
char cell_get_nb_alive_neighbours(cell *target);
void game_next(game *head);
void randomize_grid(game *target);
cell **manage_process_queue(chain_cells *queue_start, unsigned int *nb_items);
cell *get_top_right_cell(game *target);
void extend_grid(game *target, direction d);
void init_cell_chunk_top(game *target);
void merge_lines (cell *l1, cell *l2, direction end_is_to_the);
bool cell_is_available(cell *target, direction additionnal);
void merge_cols(cell *l1, cell *l2, direction from_the);
void create_col(cell *start, signed long from_y, signed long until_y);


#endif