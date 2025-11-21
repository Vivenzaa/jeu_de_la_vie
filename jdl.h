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


typedef struct game_s
{
    cell ***grille;
    unsigned int x;
    unsigned int y;
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
cell *init_targeted_square(unsigned int y, unsigned int x);
game *init_game_default(void);
void cell_free(cell *target);
void game_free(game *target);
void game_print(game *target, WINDOW *win);
void game_edit_square(cell *target, char square, game *game);
char cell_get_nb_alive_neighbours(cell *target);
void game_next(game *head);
void randomize_grid(game *target);
cell **manage_process_queue(chain_cells *queue_start, unsigned int *nb_items);



#endif