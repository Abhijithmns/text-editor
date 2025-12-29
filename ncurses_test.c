#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ncurses.h>

#define MAX_COLS 256
#define MAX_ROWS 1000

typedef enum {
    NORMAL,
    INSERT,
    COMMAND
} MODE;

char buffer[MAX_ROWS][MAX_COLS];
int num_lines = 1;
int cx = 0, cy = 0; //initial position of the cursor
MODE mode = NORMAL;
char command[32];
int cmd_len = 0;

void draw_screen() {
    clear();
    int rows, cols;
    getmaxyx(stdscr, rows, cols);
    
    //for text
    int visible_lines = (num_lines < rows - 1) ? num_lines : rows - 1;
    for(int i = 0; i < visible_lines; i++) {
        mvprintw(i, 0, "%s", buffer[i]);
    }
    
    //for status bar
    attron(A_REVERSE);
    mvhline(rows - 1, 0, ' ', cols);
    
    if(mode == COMMAND) {
        mvprintw(rows - 1, 0, ":%s", command);
    } else {
        mvprintw(rows - 1, 0,
             "-- %s --  Ln %d, Col %d",
             mode == INSERT ? "INSERT" :
             mode == NORMAL ? "NORMAL" : "COMMAND",
             cy + 1,
             cx + 1);
    }
    attroff(A_REVERSE);
    
    if(mode == COMMAND) {
        move(rows - 1, cmd_len + 1);
    } else {
        move(cy, cx);
    }
    
    refresh();
}

void insert_char(int ch) {
    int len = strlen(buffer[cy]);
    if(len < MAX_COLS - 1) {
        memmove(&buffer[cy][cx + 1], &buffer[cy][cx], len - cx + 1); //shift everything to right;
        buffer[cy][cx] = ch;
        cx++;
    }
}

void delete_char() {
    if(cx > 0) {
        int len = strlen(buffer[cy]);
        memmove(&buffer[cy][cx - 1], &buffer[cy][cx], len - cx + 1);
        cx--;
    }
}

int main() {
    initscr(); //start ncurses session
    raw(); //can use cbreak();
    keypad(stdscr, TRUE);
    noecho();
    
    buffer[0][0] = '\0';
    draw_screen();
    
    int ch;
    while(1) {
        ch = getch();
        
        if(mode == NORMAL) {
            switch(ch) {
                //vim motions
                case 'h':
                    if(cx > 0) cx--;
                    break;
                case 'l':
                    if(cx < (int)strlen(buffer[cy])) cx++;
                    break;
                case 'k':
                    if(cy > 0) {
                        cy--;
                        if(cx > (int)strlen(buffer[cy]))
                            cx = strlen(buffer[cy]);
                    }
                    break;
                case 'j':
                    if(cy < num_lines - 1) {
                        cy++;
                        if(cx > (int)strlen(buffer[cy]))
                            cx = strlen(buffer[cy]);
                    }
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
        else if(mode == INSERT) {
            if(ch == 27) {        // if esc key is pressed
                mode = NORMAL;
            }
            else if(ch == KEY_BACKSPACE || ch == 127) {
                delete_char();
            }
            else if(ch == '\n') {
                if(num_lines >= MAX_ROWS) continue;
                memmove(&buffer[cy + 2], &buffer[cy + 1], (num_lines - cy - 1) * sizeof(buffer[0])); //all existing lines must move down by one
                strcpy(buffer[cy + 1], &buffer[cy][cx]); //copy everything to a newline
                buffer[cy][cx] = '\0';
                cy++;   //make the cursor point to new line strarting position
                cx = 0;
                num_lines++;
            }
            else if(ch >= 32 && ch <= 126) {
                insert_char(ch);
            }
        }
        else if(mode == COMMAND) {
            if(ch == 27) {        // if esc key is pressed
                mode = NORMAL;
            }
            else if(ch == '\n') {
                if(strcmp(command, "q") == 0) {
                    break;
                }
                mode = NORMAL;
            }
            else if(ch == KEY_BACKSPACE || ch == 127) {
                if(cmd_len > 0) {
                    command[--cmd_len] = '\0';
                }
            }
            else if(ch >= 32 && ch <= 126) {
                if(cmd_len < (int)sizeof(command) - 1) {
                    command[cmd_len++] = ch;
                    command[cmd_len] = '\0';
                }
            }
        }
        
        draw_screen();
    }
    
    endwin();
    return 0;
}
