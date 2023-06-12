/* Conway's Game of Life by Isaac Jung
Last updated 06/12/2023

|===========================================================================================================|
|   This header file contains macros and default global constants for the main game.                        |
|===========================================================================================================|
*/

// for clearing the screen
#define clear() printf("\033[H\033[J")

// global constants for the size of the field of life
const int MAX_ROWS = 30;
const int MAX_COLS = 60;

// global constant for the number of rounds the game should last, if user does not specify
const int TOTAL_ROUNDS_DEFAULT = 1000;

// global constant for the how dense the population will be initially
const int INIT_POP_VAL = 3; // higher number will decrease the initial population

// global constants for calculations of deaths and births
const int MIN_POP_SURVIVAL = 2;	// a live cell must have between the minimum and maximum live
const int MAX_POP_SURVIVAL = 3;	// 	neighbors (inclusive) to survive into the next round
const int MIN_POP_BIRTH = 3;	// a dead cell with between the minimum and maximum live neighbors
const int MAX_POP_BIRTH = 3;	// 	(inclusive) will come alive in the next round