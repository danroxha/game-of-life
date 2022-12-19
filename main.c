#include<stdio.h>
#include<stdbool.h>
#include<stdlib.h>
#include<time.h>
#include<mpi.h>

#include"screen.h"
#include"keyboard.h"
#include"gfl.h"


int main(int argc, char** argv) {

  int rank;

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Datatype mpi_game_of_life_type;
  struct_definition_game_of_life_mpi(&mpi_game_of_life_type);

  // if(rank == ROOT_PROCESS) {
  //   nocursor();
  //   clear();
  // }

  struct game_of_life gfl;
  int factor = 0;

  struct winsize ws;;

  if(rank == ROOT_PROCESS) {

    if(argc >= 2) {
      factor = atoi(argv[1]);
    }

    if(factor == 0) {
      factor = 10;
    }

    gfl = new_game_of_life(factor);
    ws.ws_row = 35;// gfl.lines;
    ws.ws_col = 150;// gfl.collumns;
  }
  
  int key;
  int generation = 0;

  bool loop = true;

  int screen_x = 0;
  int screen_y = 0;

  bool refresh = false;

  double start_time, end_time;
  while(loop) {
    start_time = MPI_Wtime();
    
    apply_rules(&gfl, mpi_game_of_life_type);
    update_generation(&gfl);
    
    end_time = MPI_Wtime();
  
    if(rank != ROOT_PROCESS) {
      continue;
    }

    gotoxy(0, LINE + 1);
    printf("BOARD[%d x %d]:[%d, %d]: Geração: %d - Move Screen(A S D W) - Reset (R) - Clear Screen (H) - %f", LINE, COL, screen_x, screen_y, generation, end_time - start_time); 

    render_board(gfl, screen_x, screen_y, ws, refresh);
 
    generation++;
    refresh = false;
  }

  if(rank == ROOT_PROCESS) {
    showcursor();
    reset_video(); 
  }
  MPI_Finalize();
  
  return 0;
}

