//stage-1 
// 1)Non -canaonical input form:
//-->Canonical mode: Terminal waits for Enter before sending input (line-buffered).
//-->Non-canonical mode: Each key is sent immediately, allowing real-time interaction (used in text editors & games).
//2)Turning off ECHO:
//-->ECHO makes the terminal automatically show what you type.
//-->Text editors turn it off so they can control the screen and draw text themselves.
//3)Proper reset on exit(both for normal and abnormal exit):
//-->handles if the terminal thing breaks
// STAGE 2:
// cursor movement
// STAGE 3:
// optimize rendering
// optimize the loop
#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <signal.h>
#include <stdlib.h>
#define SPEED 0.1 //Lower is better
#define MAX(a,b) ((a)>(b) ? (a) : (b))
#define MIN(a,b) ((a)>(b) ? (b) : (a))
#define MAX_X 60
#define MAX_Y 26 //just for testing

typedef struct {
    int key;
    int pos_x;
    int pos_y;
    char old_screen[MAX_Y][MAX_X]; //
    char screen[MAX_Y][MAX_X];
}cursorState;

static struct termios old_termios,new_termios; // static remembers the prev values

void reset_terminal() {
    printf("\e[?25h"); //show cursor
    fflush(stdout);
    tcsetattr(STDIN_FILENO,TCSANOW,&old_termios);
//    printf("\e[m"); //reset color changes (dont need this for now)
}

void signal_handler(int signum){
    reset_terminal();
    signal(signum,SIG_DFL);
    raise(signum);
}

void configure_terminal() {
    tcgetattr(STDIN_FILENO,&old_termios); //this function stores the current snapshot of the terminal
                                          //old_termios is a struct  
    new_termios = old_termios; //saving a copy . when we reset the terminal we can bring everything back to normal
    new_termios.c_lflag &= ~(ICANON | ECHO); //(ICANON | ECHO) means "a bitmask that has both ICANON and ECHO set to 1"(bitwise OR)
                                             //bitmask is just a number used to manipulate specific bits inside another number:
                                            //like to Turn bits on (set to 1),Turn bits off (set to 0) or to check if its on or off
    new_termios.c_cc[VMIN] = 0;             //VMIN and VTIME are the fields insider termios struct.
                                            //It defines how many characters must be available before read() returns.
                                            //VTIME : It defines how long read() should wait for input before giving up(it is measures in 0.1's)
    new_termios.c_cc[VTIME] = 0;
    tcsetattr(STDIN_FILENO,TCSANOW,&new_termios); //setting the attributes like ECHO and ICANON (off) and TCSANOW:apply immediately
    printf("\e[?25l"); //hide cursor
   atexit(reset_terminal);

}

int read_key(char *buffer,int k){
    if(buffer[k] == '\033' && buffer[k+1] == '['){
        switch(buffer[k+2]){
            case 'A': return 1;
            case 'B': return 2;
            case 'C': return 3;
            case 'D': return 4;
        }
    }
    return 0;
}

int read_input() {
    char buffer[4096]; //Maximum input buffer stored in the terminal
    int n = read(STDIN_FILENO,buffer,sizeof(buffer)); //returns the number of bytes read
    int final_key = 0;
    for(int k=0;k<=n-3;k+=3){ //n - 3 ensures the 3-byte sequence buf[k], buf[k+1], buf[k+2] never goes out of bounds also we are handling arrow keys for now (they are 3 char sequences)
       int key  = read_key(buffer,k);
        if(key == 0) {
            continue;
        }
        final_key = key;
    }
    return final_key;
}

void print_key(int key){
    if (key == 1) printf("Up\n");
    if (key == 2) printf("Down\n");
    if (key == 3) printf("Right\n");
    if (key == 4) printf("Left\n");
}

void handle_cursor(int key,int *pos_x,int *pos_y){
    switch(key){
        case 1: //moving up
            *pos_y = MAX(1,*pos_y-1);
            break;
        case 2:
            *pos_y = MIN(MAX_Y - 2,*pos_y+1); // -2 because we leave some space for the border
            break;
        case 3:    
            *pos_x = MIN(MAX_X - 3,*pos_x+1); // -3 for newline charecter and the border
            break;
        case 4:
            *pos_x = MAX(1,*pos_x-1);
            break;
        default: break;
    }    
}

void render(int pos_x,int pos_y){
    printf("\e[2J"); //clearing the screen
    printf("\e[1;1H"); //positioning the cursor
    for(int i =0;i<MAX_X - 1;++i){
        printf("X");
    }
    printf("\n");
    for(int i =1;i<MAX_Y -1;++i){
        printf("X");
        for(int j=1;j<MAX_X-2;++j){
           if(pos_x == j && pos_y == i) {
                printf("|");
            }
            else{
                printf(" ");
            }
        }
        printf("X");
        printf("\n");
    }
    for(int i =0;i<MAX_X - 1;++i){
        printf("X");
    }
    printf("\n");
    fflush(stdout);
}
/*
* 
- Position the Cursor:
  \033[<L>;<C>H
     Or
  \033[<L>;<C>f
  puts the cursor at line L and column C.
- Move the cursor up N lines:
  \033[<N>A
- Move the cursor down N lines:
  \033[<N>B
- Move the cursor forward N columns:
  \033[<N>C
- Move the cursor backward N columns:
  \033[<N>D

- Clear the screen, move to (0,0):
  \033[2J
- Erase to end of line:
  \033[K

- Save cursor position:
  \033[s
- Restore cursor position:
  \033[u
*/


int main(){
    configure_terminal(); 
    signal(SIGINT,signal_handler);
    struct timespec req = {}; //req = how long you want to sleep
    struct timespec rem = {}; // rem = how much time was left if the sleep was interrupted (optional)
    int pos_x = 1,pos_y = 1;
    while(1){
        int key = read_input();
       // print_key(key);
        

        handle_cursor(key,&pos_x,&pos_y);
        render(pos_x,pos_y);
        req.tv_nsec = SPEED * 1000000000;
        nanosleep(&req,&rem);
    }
    

}
