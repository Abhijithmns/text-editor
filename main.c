//stage-1 
// 1)Non -canaonical input form:
//-->Canonical mode: Terminal waits for Enter before sending input (line-buffered).
//-->Non-canonical mode: Each key is sent immediately, allowing real-time interaction (used in text editors & games).
//2)Turning off ECHO:
//-->ECHO makes the terminal automatically show what you type.
//-->Text editors turn it off so they can control the screen and draw text themselves.
//3)Proper reset on exit(both for normal and abnormal exit):
//-->handles if the terminal thing breaks
#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <signal.h>
#include <stdlib.h>

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


int main(){
    configure_terminal(); 
    signal(SIGINT,signal_handler);
    struct timespec req = {}; //req = how long you want to sleep
    struct timespec rem = {}; // rem = how much time was left if the sleep was interrupted (optional)
    while(1){
        int key = read_input();
        print_key(key);

        req.tv_nsec = 0.1 * 1000000000;
        nanosleep(&req,&rem);
    }
    

}
