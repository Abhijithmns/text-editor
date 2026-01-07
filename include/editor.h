#ifndef EDITOR_H
#define EDITOR_H

#include <ncurses.h>
#include "gap_buffer.h"

typedef enum {
    NORMAL,
    INSERT,
    COMMAND
} MODE;

void editor_init(void);
void editor_cleanup(void);
void editor_loop(void);

#endif

