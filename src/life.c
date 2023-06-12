/* Conway's Game of Life by Isaac Jung
Last updated 06/12/2023

|===========================================================================================================|
|   I'll get around to writing all the documentation later... eventually.                                   |
|===========================================================================================================|
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

// for clearing the screen
#define clear() printf("\033[H\033[J")

// global constants for the size of the field of life
const int MAX_ROWS = 30;
const int MAX_COLS = 60;

// global constant for the number of rounds the game should last, if user does not specify
const int TOTAL_ROUNDS_DEFAULT = 1000;

// global constant for the how dense the population will be initially
const int INIT_POP_VAL = 3;	// higher number will decrease the initial population

// global constants for calculations of deaths and births
const int MIN_POP_SURVIVAL = 2;	// a live cell must have between the minimum and maximum live
const int MAX_POP_SURVIVAL = 3;	// 	neighbors (inclusive) to survive into the next round
const int MIN_POP_BIRTH = 3;	// a dead cell with between the minimum and maximum live neighbors
const int MAX_POP_BIRTH = 3;	// 	(inclusive) will come alive in the next round

// forward declarations of functions used by main or its subroutines
int init_field(char *field);					// sets up the field of life
void disp_field(char *field, int *stats, int round);		// outputs the current field to console
int update_field(char *field, int *stats);			// updates the field and stats
	int get_neighbor_count(char *field, int i, int j);	// counts living neighbors

// main method, defines overall logic
int main(int argc, char *argv[])
{
	int total_rounds = TOTAL_ROUNDS_DEFAULT;
	if (argc >= 2) {
		total_rounds = atoi(argv[1]);
		if (total_rounds == 0) total_rounds = TOTAL_ROUNDS_DEFAULT;
	}

	// set up the field
	char field[MAX_ROWS*MAX_COLS];
	int stats[3] = {0, 0, 0};	// current population, total deaths, total births
	stats[0] = init_field(field);

	// run the game
	printf("Running Game of Life for %d rounds...\n\n", total_rounds);
	int round;
	for (round = 0; round < total_rounds; round++) {
		disp_field(field, stats, round);
		sleep(1);
		stats[0] += update_field(field, stats);
	}
	disp_field(field, stats, round);

	printf("\nGAME FINISHED\n");
	return 1;
}

// subroutine to initialize the field of life with some cells alive, some dead
//  - returns the initial population
int init_field(char *field)
{
	srand(time(0));
	int i, j, cur, cnt = 0;
	for (i = 0; i < MAX_ROWS; i++) {
		for (j = 0; j < MAX_COLS; j++) {
			cur = random()%INIT_POP_VAL;
			if (cur == 0) {
				field[i*MAX_COLS+j] = '*';
				cnt++;	// increment the cnt for total population
			} else field[i*MAX_COLS+j] = ' ';
		}
	}
	return cnt;
}

// subroutine to handle the display of the field of life for a given round, via mapping
void disp_field(char *field, int *stats, int round)
{
	clear();
	int i, j;
	char disp[MAX_ROWS+2][MAX_COLS+1];

	// fill out disp with strings for later printing
	for (j = 0; j < MAX_COLS; j++) {
		disp[0][j] = '=';
		disp[MAX_ROWS+1][j] = '=';
	}
	disp[0][MAX_COLS] = '\0';
	for (i = 1; i < MAX_ROWS+1; i++) {
		for (j = 0; j < MAX_COLS; j++) {
			disp[i][j] = field[(i-1)*MAX_COLS+j];
		}
		disp[i][MAX_COLS] = '\0';
	}
	disp[MAX_ROWS+1][MAX_COLS] = '\0';

	// print out field using disp
	printf("\033[1;0H =%s=\n", &disp[0][0]);
	for (i = 1; i < MAX_ROWS+1; i++) {
		printf("\033[%d;0H |%s|\n", i+1, &disp[i][0]);	// print left border
	}
	printf("\033[%d;0H =%s=\n", MAX_ROWS+2, &disp[MAX_ROWS+1][0]);

	// print stats
	printf("     Current Round = %9d    Current Alive = %9d\n      Total Deaths = %9d     Total Births = %9d\n", round, stats[0], stats[1], stats[2]);
}

// subroutine to update the internal board between rounds
// - returns the net change in population
int update_field(char *field, int *stats)
{
	int i, j, cnt, net_change = 0;
	char tmp[MAX_ROWS][MAX_COLS];

	// store changes into a temporary field based on neighbor counts for each cell
	for (i = 0; i < MAX_ROWS; i++) {
		for (j = 0; j < MAX_COLS; j++) {
			cnt = get_neighbor_count(field, i, j);
			if (field[i*MAX_COLS+j] == '*') {
				if (cnt >= MIN_POP_SURVIVAL && cnt <= MAX_POP_SURVIVAL) {
					tmp[i][j] = '*';
				} else {
					tmp[i][j] = ' ';
					net_change--;
					stats[1]++;
				}
			} else {
				if (cnt >= MIN_POP_BIRTH && cnt <= MAX_POP_BIRTH) {
					tmp[i][j] = '*';
					net_change++;
					stats[2]++;
				} else {
					tmp[i][j] = ' ';
				}
			}

		}
	}

	// update main field with changes
	for (i = 0; i < MAX_ROWS; i++) {
		for (j = 0; j < MAX_COLS; j++) {
			field[i*MAX_COLS+j] = tmp[i][j];
		}
	}
	return net_change;
}

// helper function to get the count of living neighbors for a single cell
//  - deals with edge cases by wrapping around to the other side of the field
int get_neighbor_count(char *field, int i, int j)
{
	int cnt = 0;

	// check upper-left
	if (i == 0 && j == 0) {
		if (field[(MAX_ROWS-1)*MAX_COLS+MAX_COLS-1] == '*') cnt++;
	} else if (i == 0) {
		if (field[(MAX_ROWS-1)*MAX_COLS+j-1] == '*') cnt++;
	} else if (j == 0) {
		if (field[(i-1)*MAX_COLS+MAX_COLS-1] == '*') cnt++;
	} else {
		if (field[(i-1)*MAX_COLS+j-1] == '*') cnt++;
	}

	// check above
	if (j == 0) {
		if (field[i*MAX_COLS+MAX_COLS-1] == '*') cnt++;
	} else {
		if (field[i*MAX_COLS+j-1] == '*') cnt++;
	}

	// check upper-right
	if (i == MAX_ROWS-1 && j == 0) {
		if (field[0*MAX_COLS+MAX_COLS-1] == '*') cnt++;
	} else if (i == MAX_ROWS-1) {
		if (field[0*MAX_COLS+j-1] == '*') cnt++;
	} else if (j == 0) {
		if (field[(i+1)*MAX_COLS+MAX_COLS-1] == '*') cnt++;
	} else {
		if (field[(i+1)*MAX_COLS+j-1] == '*') cnt++;
	}

	// check left
	if (i == 0) {
		if (field[(MAX_ROWS-1)*MAX_COLS+j] == '*') cnt++;
	} else {
		if (field[(i-1)*MAX_COLS+j] == '*') cnt++;
	}

	// check right
	if (i == MAX_ROWS-1) {
		if (field[0*MAX_COLS+j] == '*') cnt++;
	} else {
		if (field[(i+1)*MAX_COLS+j] == '*') cnt++;
	}

	// check bottom left
	if (i == 0 && j == MAX_COLS-1) {
		if (field[(MAX_ROWS-1)*MAX_COLS+0] == '*') cnt++;
	} else if (i == 0) {
		if (field[(MAX_ROWS-1)*MAX_COLS+j+1] == '*') cnt++;
	} else if (j == MAX_COLS-1) {
		if (field[(i-1)*MAX_COLS+0] == '*') cnt++;
	} else {
		if (field[(i-1)*MAX_COLS+j+1] == '*') cnt++;
	}

	// check below
	if (j == MAX_COLS) {
		if (field[i*MAX_COLS+0] == '*') cnt++;
	} else {
		if (field[i*MAX_COLS+j+1] == '*') cnt++;
	}

	// check bottom-right
	if (i == MAX_ROWS && j == MAX_COLS) {
		if (field[0*MAX_COLS+0] == '*') cnt++;
	} else if (i == MAX_ROWS) {
		if (field[0*MAX_COLS+j+1] == '*') cnt++;
	} else if (j == MAX_COLS) {
		if (field[(i+1)*MAX_COLS+0] == '*') cnt++;
	} else {
		if (field[(i+1)*MAX_COLS+j+1] == '*') cnt++;
	}

	return cnt;
}
