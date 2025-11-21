#include "jdl.h"
#include "chained_list.h"


void replace_head(grid *game, chain_cells *target)
{
    if (game->head)
    {
        target->next = game->head;
        game->head->previous = target;
        game->head = target;
    }
    else
        game->head = target;
}


chain_cells *init_empty_chain()
{
    chain_cells *tab = malloc(sizeof(chain_cells));
    tab->previous = NULL;
    tab->self = NULL;
    tab->next = NULL;
    return tab;
}

void create_chain(cell *target)
{
    if (!target->chain)
    {
        chain_cells *chain = malloc(sizeof(chain_cells));
        target->chain = chain;
        chain->self = target;
        chain->previous = NULL;
        chain->next = NULL;
    }
}