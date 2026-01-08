#include <ncurses.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "editor.h"
#include "gap_buffer.h"

int main(int argc,char **argv) {

    initscr(); // start ncurses session 
    raw();     // can use cbreak(); 
    keypad(stdscr, TRUE);
    noecho();

    editor_init();

    if(argc > 1) {
        FILE *file = fopen(argv[1],"r");
        if(file) {
            gb_load_file(gb,file);
            fclose(file);
            cursor = gb_point_offset(gb);

        }
        current_file = strdup(argv[1]); //stores the filename,(strdup creates a duplicate of a string by allocating new memory for the copy)
    }

    draw_screen();// initial draw after the file load

    editor_loop();
    editor_cleanup();

    if(current_file) {
        free(current_file);
    }

    endwin();
    return 0;
}

