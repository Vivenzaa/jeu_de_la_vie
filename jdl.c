#include "jdl.h"



cell *init_targeted_square(unsigned int x, unsigned int y)
{
    cell *target = malloc(sizeof(cell));
    target->top         = NULL;
    target->bot         = NULL;
    target->left        = NULL;
    target->right       = NULL;
    target->top_left    = NULL;
    target->top_right   = NULL;
    target->bot_left    = NULL;
    target->bot_right   = NULL;
    target->chain       = NULL;
    target->next_value = DEAD;
    target->value = DEAD;
    target->x = x;
    target->y = y;
    target->will_be_processed = 0;

    return target;
}

grid *create_grid(void)
{
    grid *game = init_grid_default();
    randomize_grid(game);

    return game;
}

grid *init_grid_default(void)
{
    grid *target = malloc(sizeof(grid));
    
    target->head = NULL;
    target->x = 10;
    target->y = 10;
    char top_condition = 0;
    char bot_condition = 0;
    char left_condition = 0;
    char right_condition = 0;    

    target->grille = malloc(sizeof(cell **) * target->x);
    for (unsigned int i = 0; i < target->x ; i++)
    {
        target->grille[i] = malloc(target->y * sizeof(cell *));
        for (unsigned int j = 0 ; j < target->y ; j++)
            target->grille[i][j] = init_targeted_square(i, j);
    }
        

    // on peut pas intégrer le malloc dans la boucle d'après pck sinon on ne peut pas assigner les i+1 etc

    for (unsigned int i = 0; i < target->x ; i++)
    { 
        for (unsigned int j = 0 ; j < target->y ; j++)
        {
            
            top_condition = i > 0;
            bot_condition = i < target->x - 1;
            left_condition = j > 0;
            right_condition = j < target->y - 1;

            // -------------------------------------- assigns neighbours if they exist --------------------------------------
            if (top_condition)                          target->grille[i][j]->top           = target->grille[i-1]   [j];
            if (bot_condition)                          target->grille[i][j]->bot           = target->grille[i+1]   [j];
            if (left_condition)                         target->grille[i][j]->left          = target->grille[i]     [j-1];
            if (right_condition)                        target->grille[i][j]->right         = target->grille[i]     [j+1];

            if (top_condition && left_condition)        target->grille[i][j]->top_left      = target->grille[i-1]   [j-1];
            if (top_condition && right_condition)       target->grille[i][j]->top_right     = target->grille[i-1]   [j+1];
            if (bot_condition && left_condition)        target->grille[i][j]->bot_left      = target->grille[i+1]   [j-1];
            if (bot_condition && right_condition)       target->grille[i][j]->bot_right     = target->grille[i+1]   [j+1];
            // -------------------------------------- assigns neighbours if they exist --------------------------------------
        }
    }
    return target;
}

void square_free(cell *target)
{
    if (target->chain)
        free(target->chain);
    free(target);
}


void grid_free(grid *target)
{
    if (!target)    return;
    for (unsigned int i = 0 ; i < target->x ; i++)
    {
        for (unsigned int j = 0 ; j < target->y ; j++)
            square_free(target->grille[i][j]);
        free(target->grille[i]);
    }
    free(target->grille);
    free(target);
}


void grid_print(grid *target, WINDOW *win)
{
    for (unsigned int x = 0 ; x < target->x ; x++)
    {
        for (unsigned int y = 0 ; y < target->y ; y++)
            /*if (target->grille[x][y]->chain)
                mvwprintw(win, x, y*2, "%ls", L"🟥");
            else*/
                mvwprintw(win, x, y*2, "%ls%ls", 
                                    BLACK + (1 *  (target->grille[x][y]->value)),      // affiche BLACK si target->grille[x][y] == 0
                                    WHITE + (1 * !(target->grille[x][y]->value)));     // affiche WHITE si target->grille[x][y] == 1
        
            
    }
    refresh();
    return;
}


void grid_edit_square(cell *target, char square, grid *game)
{
    target->value = square;
    if (square)
    {
        if (!target->chain)
        {
            create_chain(target);
            replace_head(game, target->chain);
        }
    }
    else 
    {
        if (target->chain != NULL)
        {
            if (target->chain->next)
            target->chain->next->previous = target->chain->previous;       // le previous du prochain devient le previous actuel

            if (target->chain->previous)
                target->chain->previous->next = target->chain->next;           // le next du previous devient le next actuel
            else
                game->head = game->head->next;
            
            free(target->chain);
            target->chain = NULL;
        }
    }
}


char cell_get_nb_alive_neighbours(cell *target)
{
    if (!target)    return 0;
    char i = 0;

    if (target->top)            i += target->top->value;
    if (target->bot)            i += target->bot->value;
    if (target->left)           i += target->left->value;
    if (target->right)          i += target->right->value;
    if (target->top_left)       i += target->top_left->value;
    if (target->top_right)      i += target->top_right->value;
    if (target->bot_left)       i += target->bot_left->value;
    if (target->bot_right)      i += target->bot_right->value;
    
    return i;
}


void grid_next(grid *game)
{
    cell **process_queue = NULL;
    unsigned int last_element = 0;
    if (game->head)
        process_queue = manage_process_queue(game->head, &last_element);
    else
    {
        process_queue = calloc(game->x * game->y, sizeof(cell *));
        for (unsigned int i = 0; i < game->x; i++)
        {
            for (unsigned int j = 0; j < game->y; j++)
                process_queue[last_element++] = game->grille[i][j];
        }
    }

    for (unsigned int i =0; i < last_element; i++)
    {
        char nb_alive_neighbours = cell_get_nb_alive_neighbours(process_queue[i]);
        process_queue[i]->next_value = (nb_alive_neighbours == 3) || (process_queue[i]->value && nb_alive_neighbours == 2);
        process_queue[i]->will_be_processed = 0;
    }
    for (unsigned int i=0; i < last_element; i++)
        grid_edit_square(process_queue[i], process_queue[i]->next_value, game);
    
    free(process_queue);
    process_queue = NULL;
    last_element = 0;
}



cell **manage_process_queue(chain_cells *queue_start, unsigned int *nb_items)
{
    unsigned int i = 0;
    unsigned int size_of_queue = 2048;
    cell **to_process = calloc(size_of_queue, sizeof(cell *));
    chain_cells *current_chain = queue_start;
    cell *current_cell = NULL;
    
    while(current_chain)
    {
        current_cell = current_chain->self;

        if (i+8 >= size_of_queue)
        {
            size_of_queue *= 2;
            to_process = realloc(to_process, sizeof(cell *) * size_of_queue);
        }

        if (!current_cell->will_be_processed)
        {
            current_cell->will_be_processed++;
            to_process[i++] = current_cell;
        }

        if (current_cell->top_left && !current_cell->top_left->will_be_processed)
        {
            to_process[i++] = current_cell->top_left;
            current_cell->top_left->will_be_processed++;
        }

        if (current_cell->top && !current_cell->top->will_be_processed)
        {
            to_process[i++] = current_cell->top;
            current_cell->top->will_be_processed++;
        }

        if (current_cell->top_right && !current_cell->top_right->will_be_processed)
        {
            to_process[i++] = current_cell->top_right;
            current_cell->top_right->will_be_processed++;
        }

        if (current_cell->left && !current_cell->left->will_be_processed)
        {
            to_process[i++] = current_cell->left;
            current_cell->left->will_be_processed++;
        }

        if (current_cell->right && !current_cell->right->will_be_processed)
        {
            to_process[i++] = current_cell->right;
            current_cell->right->will_be_processed++;
        }

        if (current_cell->bot_left && !current_cell->bot_left->will_be_processed)
        {
            to_process[i++] = current_cell->bot_left;
            current_cell->bot_left->will_be_processed++;
        }

        if (current_cell->bot && !current_cell->bot->will_be_processed)
        {
            to_process[i++] = current_cell->bot;
            current_cell->bot->will_be_processed++;
        }

        if (current_cell->bot_right && !current_cell->bot_right->will_be_processed)
        {
            to_process[i++] = current_cell->bot_right;
            current_cell->bot_right->will_be_processed++;
        }

        current_chain = current_chain->next;
    }

    *nb_items = i;
    return to_process;
}


void randomize_grid(grid *target)
{
    FILE *r = fopen("/dev/urandom", "r");

    for (unsigned int i=0 ; i < target->x ; i++)
    {
        for (unsigned int j=0 ; j < target->y ; j++)
            grid_edit_square(target->grille[i][j], fgetc(r)%2, target);
    }

    fclose(r);
}


