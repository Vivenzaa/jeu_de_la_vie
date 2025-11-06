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
    target->next_value = DEAD;
    target->value = DEAD;
    target->x = x;
    target->y = y;

    return target;
}


grid *init_grid_default(void)
{
    grid *target = malloc(sizeof(grid));
    target->x = 10;
    target->y = 10;
    char top_condition = 0;
    char bot_condition = 0;
    char left_condition = 0;
    char right_condition = 0;    

    target->grille = malloc(sizeof(cell **) * target->x);
    for (unsigned int i = 0; i < target->x ; i++)
        target->grille[i] = malloc(target->y * sizeof(cell *));

    // on peut pas intégrer le malloc dans la boucle d'après pck sinon on ne peut pas assigner les i+1 etc

    for (unsigned int i = 0; i < target->x ; i++)
    { 
        for (unsigned int j = 0 ; j < target->y ; j++)
        {
            target->grille[i][j] = init_targeted_square(i, j);
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
    if (!target)    return;
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
            mvwprintw(win, x, y*2, "%ls%ls", 
                                    BLACK + (1 *  (target->grille[x][y]->value)),      // affiche BLACK si target->grille[x][y] == 0
                                    WHITE + (1 * !(target->grille[x][y]->value)));     // affiche WHITE si target->grille[x][y] == 1
        
            
    }
    refresh();
    return;
}



void grid_edit_square(grid *target, unsigned int x, unsigned int y, char square)
{
    if (target && x <= target->x && y <= target->y)
        target->grille[x][y]->value = !!square;    // (!! pour eviter le cas > 1)
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




void grid_next(grid *target)
{
    for (unsigned int i = 0 ; i < target->x ; i++)
    {
        for (unsigned int j = 0 ; j < target->y ; j++)
        {
            char nb_alive_neighbours = cell_get_nb_alive_neighbours(target->grille[i][j]);      
            target->grille[i][j]->next_value = (nb_alive_neighbours == 3) || (target->grille[i][j]->value && nb_alive_neighbours == 2); 
        }
    }

    for (unsigned int i = 0 ; i < target->x ; i++)
    {
        for (unsigned int j = 0 ; j < target->y ; j++)
            target->grille[i][j]->value = target->grille[i][j]->next_value;
    }
}


void randomize_grid(grid *target)
{
    FILE *r = fopen("/dev/urandom", "r");

    for (unsigned int i=0 ; i < target->x ; i++)
    {
        for (unsigned int j=0 ; j < target->y ; j++)
            target->grille[i][j]->value = fgetc(r)%2;   
    }

    fclose(r);
}