#ifndef __CHAINED_LIST_H__
#define __CHAINED_LIST_H__

#include "jdl.h"


typedef struct chain_cells_s
{
    struct chain_cells_s *previous;
    cell *self;
    struct chain_cells_s *next;
} chain_cells;



void create_chain(cell *target);
chain_cells *init_empty_chain();
void replace_head(game *game, chain_cells *target);

#endif