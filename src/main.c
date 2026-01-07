#include <ncurses.h>
#include "editor.h"

int main(void)
{
    initscr(); /* start ncurses session */
    raw();     /* can use cbreak(); */
    keypad(stdscr, TRUE);
    noecho();

    editor_init();
    editor_loop();
    editor_cleanup();

    endwin();
    return 0;
}

