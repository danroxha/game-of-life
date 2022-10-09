#include<stdio.h>
#include<stdbool.h>
#include<stdlib.h>
#include<time.h>

#include"screen.h"
#include"keyboard.h"
#include"gfl.h"

int main(int argc, char** argv) {

  init_keyboard();
  nocursor();
  clear();
  
  int factor = 0;

  if(argc >= 2) {
    factor = atoi(argv[1]);
  }

  if(factor == 0) {
    factor = 10;
  }

  struct game_of_life gfl = new_game_of_life(factor);
  
  int key;
  int generation = 0;


  struct winsize ws;
  int tty;

  tty = open("/dev/tty", O_RDWR);
  if(tty < 0 || ioctl(tty, TIOCGWINSZ, &ws) < 0) 
    err(8, "/dev/tty");

  bool loop = true;

  int screen_x = 0;
  int screen_y = 0;

  bool refresh = false;

  while(loop) {
    
    apply_rules(&gfl);


    if(tty < 0 || ioctl(tty, TIOCGWINSZ, &ws) < 0) 
      err(8, "/dev/tty");

    gotoxy(0, LINE + 1);
    printf("BOARD[%d x %d]:[%d, %d]: Geração: %d - Move Screen(A S D W) - Reset (R) - Clear Screen (H)", LINE, COL, screen_x, screen_y, generation);
    
    if (kbhit()) {
      key = readch();
      switch(key) {
        case KEY_ESC:
        case KEY_ENTER: {
          loop = false;
          break;
        }
        case KEY_R_U:
        case KEY_R_L: {
          gfl = new_game_of_life(factor);
          clear();
          generation = 0;
          continue;
        }
        case KEY_S_U:
        case KEY_S_L: {
          screen_y+=5;
          if(screen_y >= gfl.lines - ws.ws_row) {
            screen_y = gfl.lines - ws.ws_row;
          }
          refresh = true;
          break;
        }
        case KEY_W_U:
        case KEY_W_L: {
          screen_y-= 5;
          if(screen_y <= 0)
            screen_y = 0;
          refresh = true;
          break;
        }
        case KEY_D_U:
        case KEY_D_L: {
          screen_x+=5;
          if(screen_x >= gfl.collumns - ws.ws_col)
            screen_x =  gfl.collumns - ws.ws_col;
          refresh = true;
          break;
        }
        case KEY_A_U:
        case KEY_A_L: {
          screen_x-=5;
          if(screen_x <= 0)
            screen_x = 0;
          refresh = true;
          break;
        }
        case KEY_H_L:
        case KEY_H_U: {
          clear();
          break;
        }
      }
    }

    render_board(gfl, screen_x, screen_y, ws, refresh);
    update_generation(&gfl);
    generation++;
    refresh = false;
  }

  showcursor();
  close_keyboard(); 
  close(tty);
  reset_video();
  
  return 0;
}

