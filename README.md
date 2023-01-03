## ***Parallel implementaton - (OpenMP)***
# Conway's Game of Life

[click here](https://en.wikipedia.org/wiki/Conway%27s_Game_of_Life)

## Screenshot

![demo](screenshot/gun-shape.gif)

## Build and Run

### Tools and libs
- gcc version 9.4.0
- openmp

### Build
```
 mpicc main.c -o main
```

### Run
```
 mpirun -n 2 ./main

 # 2 is amount process
```

## Support
 - Linux - (Ubuntu 2024)