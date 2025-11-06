#include <stdio.h>
#include <ncursesw/ncurses.h>
#include <unistd.h>
#include <locale.h>
#include <stdlib.h>
#include <signal.h>

#include "jdl.h"


void last_second_clean(__attribute__ ((unused)) int sig)
{
    endwin();
    printf("dernier nettoyage avant la fin...\n");
    exit(0);
}

// implémenter la gestion de cluster

int main (void)
{
    signal(SIGINT, last_second_clean);
    setlocale(LC_ALL, "");
    int i = 0;
    FILE *f = fopen("log.txt", "w");
    grid *test = init_grid_default();        

    WINDOW *win = initscr ();
    curs_set (0);
    randomize_grid(test);
    
    while(1)
    {
        grid_print(test, win);
        grid_next(test);
        fprintf(f, "%d\n", i);
        i++;
    }
    fclose(f);
    grid_free(test);
    getch();
    curs_set(1);
    endwin();
    return 0;
}