#include "jdl.h"



cell *init_targeted_cell(signed long y, signed long x)
{
    cell *target = NULL;
    target = malloc(sizeof(cell));
    target->top         = NULL;
    target->bot         = NULL;
    target->left        = NULL;
    target->right       = NULL;
    target->chain       = NULL;
    target->next_value = DEAD;
    target->value = DEAD;
    target->x = x;
    target->y = y;
    target->will_be_processed = 0;

    return target;
}

game *create_game(void)
{
    game *game = init_game_default();
    randomize_grid(game);

    return game;
}

game *init_game_default(void)
{
    game *target = malloc(sizeof(game));
    signed long default_y = 3;
    signed long default_x = 7;
    
    target->head = NULL;
    cell *current = NULL;

    target->top_left = init_targeted_cell(0, 0);
    current = target->top_left;

    for (signed long y = 0; y <= default_y-1; y++)
    {
        create_line(current, 0, default_x-1);
        if (y<default_y-1)
        {
            current->bot = init_targeted_cell(y+1, 0);
            current = current->bot;
        }
    }

    while(current->right)
        current = current->right;

    target->bot_right = current;
    current = target->top_left;
    while(current->bot)
    {
        merge_lines(current, current->bot, RIGHT);
        current = current->bot;
    }
    
    return target;
}


void cell_free(cell *target)
{
    if (target->chain)
        free(target->chain);
    
    free(target);
}


void game_free(game *target)
{
    cell *current = target->top_left;
    cell *next = current->right;
    cell *left = current;
    while (left)          // <= va surement causer une segfault
    {
        while (next)
        {
            cell_free(current);
            current = next;
            next = current->right;
        }
        cell_free(current);
        left = left->bot;
        current = left;
    }

    free(target);
}

void game_print_line(WINDOW *win, cell *very_left, unsigned int x)
{
    cell *current = very_left;
    unsigned int y = 0;
    while (current)
    {
        mvwprintw(win, x, y*2, "%ls%ls", 
                                    BLACK + (1 *  (current->value)),      // affiche BLACK si target->grille[x][y] == 0
                                    WHITE + (1 * !(current->value)));     // affiche WHITE si target->grille[x][y] == 1

        y++;
        current = current->right;
    }
}


void game_print(game *target, WINDOW *win)
{
    cell *current = target->top_left;
    cell *ref_height = current;
    unsigned int x = 0;
    while (current->y < target->bot_right->y)
    {
        game_print_line(win, ref_height, x);
    
        current = ref_height->bot;
        ref_height = current;
        x++;
    }
    game_print_line(win, current, x);
    refresh();
    return;
}


void game_edit_square(cell *target, char square, game *game)
{
    target->value = square;
    if (square)
    {
        if (!target->chain)
        {
            create_chain(target);
            replace_head(game, target->chain);
        }
        if (!target->top)
            extend_grid(game, TOP);
        if (!target->bot)
            extend_grid(game, BOT);
        if (!target->left)
            extend_grid(game, LEFT);
        if (!target->right)
            extend_grid(game, RIGHT);
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

    if (target->top)
    {
        i += target->top->value;
        if (target->top->left)
            i += target->top->left->value;
        if (target->top->right)
            i += target->top->right->value;
    }       

    if (target->bot)
    {
        i += target->bot->value;
        if (target->bot->left)
            i += target->bot->left->value;
        if (target->bot->right)
            i += target->bot->right->value;
    }

    if (target->left)
        i += target->left->value;        
    if (target->right)
        i += target->right->value;         
    
    return i;
}


void game_next(game *game)
{
    cell **process_queue = NULL;
    unsigned int last_element = 0;
    if (game->head)
        process_queue = manage_process_queue(game->head, &last_element);
    else
    {
        unsigned long long size_of_process_queue = (unsigned long long)(game->bot_right->x - game->top_left->x) * (game->bot_right->y - game->top_left->y);
        if (!size_of_process_queue)
            return;
        process_queue = calloc(size_of_process_queue, sizeof(cell *));

        cell *current = game->top_left;
        cell *left = current->bot;
        while(current)
        {
            while (current)
            {
                process_queue[last_element++] = current;
                current = current->right;
            }
            current = left;
            if(left)
                left = left->bot;
        }
    }

    for (unsigned int i =0; i < last_element; i++)
    {
        char nb_alive_neighbours = cell_get_nb_alive_neighbours(process_queue[i]);
        process_queue[i]->next_value = (nb_alive_neighbours == 3) || (process_queue[i]->value && nb_alive_neighbours == 2);
        process_queue[i]->will_be_processed = 0;
    }
    for (unsigned int i=0; i < last_element; i++)
        game_edit_square(process_queue[i], process_queue[i]->next_value, game);
    
    if(process_queue)
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

        if (cell_is_available(current_cell, 0))
        {
            current_cell->will_be_processed++;
            to_process[i++] = current_cell;
        }

        if (cell_is_available(current_cell->top, LEFT))
        {
            to_process[i++] = current_cell->top->left;
            current_cell->top->left->will_be_processed++;
        }

        if (cell_is_available(current_cell->top, 0))
        {
            to_process[i++] = current_cell->top;
            current_cell->top->will_be_processed++;
        }

        if (cell_is_available(current_cell->top, RIGHT))
        {
            to_process[i++] = current_cell->top->right;
            current_cell->top->right->will_be_processed++;
        }

        if (cell_is_available(current_cell->left, 0))
        {
            to_process[i++] = current_cell->left;
            current_cell->left->will_be_processed++;
        }

        if (cell_is_available(current_cell->right, 0))
        {
            to_process[i++] = current_cell->right;
            current_cell->right->will_be_processed++;
        }

        if (cell_is_available(current_cell->bot, LEFT))
        {
            to_process[i++] = current_cell->bot->left;
            current_cell->bot->left->will_be_processed++;
        }

        if (cell_is_available(current_cell->bot, 0))
        {
            to_process[i++] = current_cell->bot;
            current_cell->bot->will_be_processed++;
        }

        if (cell_is_available(current_cell->bot, RIGHT))
        {
            to_process[i++] = current_cell->bot->right;
            current_cell->bot->right->will_be_processed++;
        }

        current_chain = current_chain->next;
    }

    *nb_items = i;
    return to_process;
}


void randomize_grid(game *target)
{
    FILE *r = fopen("/dev/urandom", "r");

    cell *current = target->top_left;
    cell *left = current->bot;

    while (current)
    {
        while (current)
        {
            game_edit_square(current, fgetc(r)%2, target);
            current = current->right;
        }
        current = left;
        if (left)
            left = left->bot;
    }

    fclose(r);
}


void create_line(cell *start, signed long from_x, signed long until_x)
{
    cell *previous = NULL;
    cell *current = start;

    if (from_x < until_x)
    {
        for (signed long x = from_x+1; x <= until_x; x++)
        {
            current->right = init_targeted_cell(current->y, x);
            previous = current;
            current = current->right;
            current->left = previous;
        }
    }
    else
    {
        for (signed long x = from_x-1; x >= until_x; x--)
        {
            current->left = init_targeted_cell(current->y, x);
            previous = current;
            current = current->left;
            current->right = previous;
        }
    }
}


void create_col(cell *start, signed long from_y, signed long until_y)
{
    cell *previous = NULL;
    cell *current = start;

    if (from_y < until_y)
    {
        for (signed long y = from_y+1; y <= until_y; y++)
        {
            current->bot = init_targeted_cell(y, current->x);
            previous = current;
            current = current->bot;
            current->top = previous;
        }
    }
    else
    {
        for (signed long y = from_y-1; y >= until_y; y--)
        {
            current->top = init_targeted_cell(y, current->x);
            previous = current;
            current = current->top;
            current->bot = previous;
        }
    }
}


void merge_lines (cell *l1, cell *l2, direction end_is_to_the)
{
    cell *under = (l1->y >= l2->y) ? l1 : l2;
    cell *upper = (l1->y < l2->y) ? l1 : l2;

    while(under)
    {
        upper->bot = under;
        under->top = upper;
        if (end_is_to_the==RIGHT)
        {
            under = under->right;
            upper = upper->right;
        }
        else
        {
            under = under->left;
            upper = upper->left;
        }
        
    }
}


void merge_cols(cell *l1, cell *l2, direction from_the)
{
    cell *right = (l1->x >= l2->x) ? l1 : l2;   // plus grand x -> est à droite
    cell *left = (l1->x < l2->x) ? l1 : l2;     // plus petit x -> est à gauche

    while(left)
    {
        left->right = right;
        right->left = left;
        if (from_the==TOP)
        {
            left = left->bot;
            right = right->bot;
        }
        else
        {
            left = left->top;
            right = right->top;
        }
    }
}

bool cell_is_available(cell *target, direction additionnal)
{
    switch(additionnal)
    {
        case RIGHT:
            return (target && target->right && !target->right->will_be_processed);

        case LEFT:
            return (target && target->left && !target->left->will_be_processed);

        default:
            return (target && !target->will_be_processed);
    }
}


void extend_grid(game *game, direction d)
{
    unsigned long height = (unsigned long)game->bot_right->y - (unsigned long)game->top_left->y + 1;
    unsigned long width = (unsigned long)game->bot_right->x - (unsigned long)game->top_left->x + 1;

    switch(d)
    {
        case TOP:
            for (unsigned long i = 0; i < height; i++)
            {
                game->top_left->top = init_targeted_cell(game->top_left->y - 1, game->top_left->x);
                create_line(game->top_left->top, game->top_left->x, game->bot_right->x);
                merge_lines(game->top_left, game->top_left->top, RIGHT);
                game->top_left = game->top_left->top;
            }
            break;
        
        case BOT:
            for (unsigned long i = 0; i < height; i++)
            {
                game->bot_right->bot = init_targeted_cell(game->bot_right->y + 1, game->bot_right->x);
                create_line(game->bot_right->bot, game->bot_right->x, game->top_left->x);
                merge_lines(game->bot_right, game->bot_right->bot, LEFT);
                game->bot_right = game->bot_right->bot;
            }
            break;

        case LEFT:
            for (unsigned long i = 0; i < width; i++)
            {
                game->top_left->left = init_targeted_cell(game->top_left->y, game->top_left->x - 1);
                create_col(game->top_left->left, game->top_left->y, game->bot_right->y);
                merge_cols(game->top_left, game->top_left->left, TOP);
                game->top_left = game->top_left->left;
            }
            break;

        case RIGHT:
            for (unsigned long i = 0; i < width; i++)
            {
                game->bot_right->right = init_targeted_cell(game->bot_right->y, game->bot_right->x + 1);
                create_col(game->bot_right->right, game->bot_right->y, game->top_left->y);
                merge_cols(game->bot_right, game->bot_right->right, BOT);
                game->bot_right = game->bot_right->right;
            }
            break;

        default:
            break;
    }
}

