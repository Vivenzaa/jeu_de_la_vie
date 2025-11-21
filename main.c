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
    signal(SIGSEGV, last_second_clean);
    setlocale(LC_ALL, "");
    int i = 0;
    FILE *f = fopen("log.txt", "w");
    grid *test = create_grid();        

    WINDOW *win = initscr ();
    curs_set (0);
    
    while(i < 10000)
    {
        //grid_print(test, win);
        grid_next(test);
        //sleep(1);
        fprintf(f, "%d\n", i);
        i++;
    }
    fclose(f);
    grid_free(test);
    getch();
    curs_set(1);
    delwin(win);
    endwin();
    return 0;
}


/*
   XXXXX
   XOOOX
   XXXXX

*/