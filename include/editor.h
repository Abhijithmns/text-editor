#ifndef EDITOR_H
#define EDITOR_H

#include <ncurses.h>
#include "gap_buffer.h"

typedef enum {
    NORMAL,
    INSERT,
    COMMAND
} MODE;

//these are for main.c function
extern GapBuffer *gb;
extern size_t cursor;
extern char *current_file;


void editor_init(void);
void editor_cleanup(void);
void editor_loop(void);
void draw_screen(void);

#endif

