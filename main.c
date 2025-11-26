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
    game *test = create_game();        

    WINDOW *win = initscr ();
    curs_set (0);
    
    while(i < 10000)
    {
        game_print(test, win);
        game_next(test);
        sleep(1);
        i++;
    }
    game_free(test);
    getch();
    curs_set(1);
    endwin();
    return 0;
}


/*
   TODO :
    - grille completement en liste chainée
    - extension de grille
    - implémentation de grille creuse

*/