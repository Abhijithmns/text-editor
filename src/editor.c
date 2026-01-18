#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ncurses.h>

#include "editor.h"

MODE mode = NORMAL;

GapBuffer *gb;
//current pos of the cursor
size_t cursor = 0;

char command[32];
char *current_file = NULL;
int cmd_len = 0;
size_t view_line = 0; //first visible line on the screen (for scroll logic)

void get_line_cols(int pos, size_t *line,size_t *col) {
    *line = 1;
    *col = 1;

    gb_set_point(gb,0);
    for(size_t i =0;i<pos;i++) {
        char c = gb_get_char(gb);

        if(c == '\n') {
            (*line)++;
            *col = 1;
        }
        else {
            (*col)++;
        }
        gb_next_char(gb);
    }
}

void MoveCursorDown() {
    size_t buf_size = gb_buffer_size(gb);

    size_t line_start = cursor; 
    while(line_start>0) {
        gb_set_point(gb,line_start - 1);
        if(gb_get_char(gb) == '\n') {
            break;
        }
        line_start--;
    }

    size_t col = cursor - line_start;

    size_t line_end = cursor;
    while(line_end < buf_size) {
        gb_set_point(gb,line_end);
        if(gb_get_char(gb) == '\n') {
            break;
        }
        line_end++;
    }

    if(line_end >= buf_size) return;

    size_t next_start = line_end + 1;

    size_t next_end = next_start;
    while(next_end < buf_size) {
        gb_set_point(gb,next_end);
        if(gb_get_char(gb) == '\n') {
            break;
        }
        next_end++;
    }

    size_t next_len = next_end - next_start;

    if(col < next_len) {
        cursor = next_start + col;
    }
    else {
        cursor = next_start + next_len;
    }
}

void MoveCursorUp() {
    size_t buf_size = gb_buffer_size(gb);

    size_t line_start = cursor;
    while(line_start > 0) {
        gb_set_point(gb,line_start-1);
        if(gb_get_char(gb) == '\n') {
            break;
        }
        line_start--;
    }

    if(line_start == 0) return;

    size_t col = cursor - line_start;

    size_t prev_end = line_start - 1;

    size_t prev_start = prev_end;
    while(prev_start > 0) {
        gb_set_point(gb,prev_start - 1);
        if(gb_get_char(gb) == '\n') {
            break;
        }
        prev_start--;
    }

    size_t prev_len = prev_end - prev_start;

    if(col < prev_len) {
        cursor = prev_start + col;
    }
    else {
        cursor = prev_start + prev_len;
    }
}

void adjust_screen() {
    int rows,cols;
    getmaxyx(stdscr,rows,cols);

    size_t cursor_line,cursor_col;
    get_line_cols(cursor,&cursor_line,&cursor_col);

    cursor_line--; //convert to zero based
    
    size_t screen_lines = rows-2; //number of that screen can display and the last line is for status line

    if(cursor_line >= view_line + screen_lines) { //cursor went down so scroll down 
        view_line = cursor_line - screen_lines + 1;
    }

    if(cursor_line < view_line) { //scroll up
        view_line = cursor_line;
    }
}

void draw_statusbar() {
    int rows, cols;
    getmaxyx(stdscr, rows, cols);

    size_t line_no, col_no;
    get_line_cols(cursor,&line_no,&col_no);

    attron(A_REVERSE);
    mvhline(rows - 1, 0, ' ', cols);

    if (mode == COMMAND) {
        mvprintw(rows - 1, 0, ":%s", command);
    }
    else {
        mvprintw(rows - 1, 0,
             "-- %s -- | Ln %zu : Col %zu | %s",
             mode == INSERT ? "INSERT" :
             mode == NORMAL ? "NORMAL" : "COMMAND",
             line_no,col_no,current_file ? current_file : "[No Name]");
    }
    attroff(A_REVERSE);
}

void place_cursor() {
    size_t cursor_line,cursor_col;
    get_line_cols(cursor,&cursor_line, &cursor_col);

    cursor_line--; // zero based for indexing

    int screen_y = (int)(cursor_line - view_line);
    int screen_x = (int)(cursor_col - 1);

    move(screen_y,screen_x);
}

void draw_screen() {
    clear();
    int rows, cols;
    getmaxyx(stdscr, rows, cols);

    // for text
    gb_set_point(gb, 0);
    size_t line = 0;

    //skip lines above view line
    while(line < view_line) {
        if(gb_get_char(gb) == '\n') {
            line ++;
        }
        gb_next_char(gb);
    }

    int y = 0, x = 0;

    //to actually print the charecters to the stdout
    while (y < rows - 1 && gb_point_offset(gb) < gb_buffer_size(gb)) {
        char c = gb_get_char(gb);

        if (c == '\n') {
            y++;
            x = 0;
        } 
        else {
            if (x >= cols) {
                y++;
                x = 0;
                if (y >= rows - 1) break;
            }
            mvaddch(y, x++, c);
        }

        gb_next_char(gb);
    }

     
    y=y+1;
    while(y < rows - 1) {
        mvaddch(y,0,'~');
        y++;
    }

    // for status bar 
    draw_statusbar();
    place_cursor();
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
                case KEY_BACKSPACE:
                    if (cursor > 0) cursor--;
                    adjust_screen();
                    break;

                case 'l':
                    if (cursor < gb_buffer_size(gb)) cursor++;
                    adjust_screen();
                    break;

                case 'k':
                    MoveCursorUp();
                    adjust_screen();
                    break;

                case 'j':
                    MoveCursorDown();
                    adjust_screen();
                    break;

                case 'g':
                    cursor = 0;
                    gb_set_point(gb,cursor);
                    view_line = 0;
                    break;

                case 'i':
                    mode = INSERT;
                    break;

                //command mode
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
                    adjust_screen();
                }
            }
            else if (ch == '\n') {
                gb_set_point(gb, cursor);
                gb_put_char(gb, '\n');
                cursor++;
                adjust_screen();
            }
            else if (ch >= 32 && ch <= 126) {
                gb_set_point(gb, cursor);
                gb_put_char(gb, (char)ch);
                cursor++;
                adjust_screen();
            }
            else if(ch == KEY_UP) {
                MoveCursorUp();
                adjust_screen();
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
                else if(strcmp(command,"wq") == 0) {
                    if(current_file) {
                        FILE *file = fopen(current_file,"w");
                        if(file) {
                            gb_save_to_file(gb,file);
                            fclose(file);
                        }
                    }
                    break;
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

