#ifndef TC_H
#define TC_H

#include <stdio.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>

#define TC_NRM  "\x1B[0m"
#define TC_RED  "\x1B[1;31m"
#define TC_GRN  "\x1B[1;32m"
#define TC_YEL  "\x1B[1;33m"
#define TC_BLU  "\x1B[1;34m"
#define TC_MAG  "\x1B[1;35m"
#define TC_CYN  "\x1B[1;36m"
#define TC_WHT  "\x1B[1;37m"

#define TC_B_NRM  "\x1B[0m"
#define TC_B_RED  "\x1B[0;31m"
#define TC_B_GRN  "\x1B[0;32m"
#define TC_B_YEL  "\x1B[0;33m"
#define TC_B_BLU  "\x1B[0;34m"
#define TC_B_MAG  "\x1B[0;35m"
#define TC_B_CYN  "\x1B[0;36m"
#define TC_B_WHT  "\x1B[0;37m"

#define TC_BG_NRM "\x1B[40m"
#define TC_BG_RED " \x1B[41m"
#define TC_BG_GRN "\x1B[42m"
#define TC_BG_YEL "\x1B[43m"
#define TC_BG_BLU "\x1B[44m"
#define TC_BG_MAG "\x1B[45m"
#define TC_BG_CYN "\x1B[46m"
#define TC_BG_WHT "\x1B[47m"

//#define tc_clear_screen() puts("\x1B[2J")
//improved clear screen
#define tc_clear_screen() write(1, "\033[H\033[2J\033[3J", 11)

#define tc_move_cursor(X, Y) printf("\033[%d;%dH", Y, X)

void tc_get_cols_rows(int *cols, int *rows);

#define tc_enter_alt_screen() puts("\033[?1049h\033[H")
#define tc_exit_alt_screen() puts("\033[?1049l")

void tc_echo_off();
void tc_echo_on();

void tc_get_cols_rows(int *cols, int *rows){

	struct winsize size;
	ioctl(1, TIOCGWINSZ, &size);
	*cols = size.ws_col;
	*rows = size.ws_row;

}//tec_get_cols_rows*/

void tc_echo_off(){

	struct termios term;
	tcgetattr(1, &term);
	term.c_lflag &= ~ECHO;
	tcsetattr(1, TCSANOW, &term);

}//tc_echo_off*/

void tc_echo_on(){

	struct termios term;
	tcgetattr(1, &term);
	term.c_lflag |= ECHO;
	tcsetattr(1, TCSANOW, &term);

}//tc_echo_on*/

#endif

