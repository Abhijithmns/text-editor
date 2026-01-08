#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ncurses.h>

#include "editor.h"

MODE mode = NORMAL;

GapBuffer *gb;
size_t cursor = 0;

char command[32];
char *current_file = NULL;
int cmd_len = 0;

void draw_screen() {
    clear();
    int rows, cols;
    getmaxyx(stdscr, rows, cols);

    // for text
    size_t saved = cursor;
    gb_set_point(gb, 0);

    int y = 0, x = 0;
    size_t pos = 0;

    while (pos < gb_buffer_size(gb) && y < rows - 1) {
        char c = gb_get_char(gb);

        if (c == '\n') {
            y++;
            x = 0;
        } else {
            mvaddch(y, x++, c);
        }

        gb_next_char(gb);
        pos++;
    }

    // for status bar 
    attron(A_REVERSE);
    mvhline(rows - 1, 0, ' ', cols);

    if (mode == COMMAND) {
        mvprintw(rows - 1, 0, ":%s", command);
    } else {
        mvprintw(rows - 1, 0,
             "-- %s --  Pos %zu",
             mode == INSERT ? "INSERT" :
             mode == NORMAL ? "NORMAL" : "COMMAND",
             cursor);
    }
    attroff(A_REVERSE);

    // restore cursor 
    gb_set_point(gb, 0);
    y = x = 0;
    for (size_t i = 0; i < cursor; i++) {
        char c = gb_get_char(gb);
        if (c == '\n') {
            y++;
            x = 0;
        } else {
            x++;
        }
        gb_next_char(gb);
    }
    move(y, x);

    refresh();
}

void editor_init(void)
{
    gb = gb_create(128);
}

void editor_cleanup(void)
{
    gb_free(gb);
}

void editor_loop(void)
{
    int ch;

    while (1) {
        draw_screen();
        ch = getch();

        if (mode == NORMAL) {
            switch (ch) {
                // vim motions 
                case 'h':
                    if (cursor > 0) cursor--;
                    break;
                case 'l':
                    if (cursor < gb_buffer_size(gb)) cursor++;
                    break;

                 //insert mode 
                case 'i':
                    mode = INSERT;
                    break;

                 //command mode (add only :q for now) 
                case ':':
                    mode = COMMAND;
                    cmd_len = 0;
                    command[0] = '\0';
                    break;
            }
        }
        else if (mode == INSERT) {
            if (ch == 27) {        // if esc key is pressed 
                mode = NORMAL;
            }
            else if (ch == KEY_BACKSPACE || ch == 127) {
                if (cursor > 0) {
                    gb_set_point(gb, cursor);
                    gb_delete_chars(gb, 1);
                    cursor--;
                }
            }
            else if (ch == '\n') {
                gb_set_point(gb, cursor);
                gb_put_char(gb, '\n');
                cursor++;
            }
            else if (ch >= 32 && ch <= 126) {
                gb_set_point(gb, cursor);
                gb_put_char(gb, (char)ch);
                cursor++;
            }
        }
        else if (mode == COMMAND) {
            if (ch == 27) {        // if esc key is pressed 
                mode = NORMAL;
            }
            else if (ch == '\n') {
                if (strcmp(command, "q") == 0) {
                    break;
                }
                else if(strcmp(command,"w") == 0) {
                    if(current_file) {
                        FILE *file = fopen(current_file,"w");

                        if(file) {
                            gb_save_to_file(gb,file);
                            fclose(file);
                        }
                    }
                }
                
                mode = NORMAL;
            }
            else if (ch == KEY_BACKSPACE || ch == 127) {
                if (cmd_len > 0) {
                    command[--cmd_len] = '\0';
                }
            }
            else if (ch >= 32 && ch <= 126) {
                if (cmd_len < (int)sizeof(command) - 1) {
                    command[cmd_len++] = ch;
                    command[cmd_len] = '\0';
                }
            }
        }

    }
}

