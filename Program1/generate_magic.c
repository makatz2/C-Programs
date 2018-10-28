////////////////////////////////////////////////////////////////////////////////
// Main File:        generate_magic.c
// This File:        generate_magic.c
// Other Files:      n/a
// Semester:         CS 354 Spring 2018
//
// Author:           Michael Katz
// Email:            makatz2@wisc.edu
// CS Login:         mkatz
//
/////////////////////////// OTHER SOURCES OF HELP //////////////////////////////
//                   fully acknowledge and credit all sources of help,
//                   other than Instructors and TAs.
//
// Persons:          Identify persons by name, relationship to you, and email.
//                   Describe in detail the the ideas and help they provided.
//
// Online sources:   avoid web searches to solve your problems, but if you do
//                   search, be sure to include Web URLs and description of 
//                   of any information you find.
////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>

// Structure representing Square
// size: dimension(number of rows/columns) of the square
// array: 2D array of integers
typedef struct _Square {
    int size;
    int **array;
} Square;

int get_square_size();
Square * generate_magic(int size);
void write_to_file(char *filename, Square * square);

int main(int argc, char *argv[])                 
{
    	// Check input arguments to get filename. Error check.
	if(argc != 2){
		fprintf(stderr, "Usage: ./generate_magic <filename>\n");
		exit(1);
	}
    	// Get size from user
	int size = get_square_size();
    	// Generate the magic square
	Square * Msquare = generate_magic(size);
    	// Write the square to the output file
	write_to_file(*(argv + 1), Msquare);
	//Free heap mem from square.
	for(int i = 0; i < Msquare->size  ; i++){
		free(*(Msquare->array + i));
	}	
	free(Msquare->array); 
	free(Msquare);
    return 0;
}

/* get_square_size prompts the user for the magic square size
 * checks if it is an odd number >= 3 and returns the number
 */
int get_square_size(){
	printf("Enter size of magic square, must be odd \n");
	int size;
	scanf("%d", &size);
	// Check that input value is correct. 
	if(size < 3 || size%2 != 1){
		fprintf(stderr, "Size must be an odd number >= 3.\n");
		exit(1);
	}
    	return size;
}

/* generate_magic constructs a magic square of size n
 * using the Siamese algorithm and returns the Square struct
 */
Square * generate_magic(int n){
	//create square object and error check. 
	Square * magic = malloc(sizeof(Square));
	if(magic == NULL){
		fprintf(stderr, "Cannot allocate memory in generate_magic");
		exit(1);
	}
	// Set size and allocate memory for array
	magic->size = n;
	magic->array = malloc(sizeof(int) * n);
	if(magic->array == NULL){
		fprintf(stderr, "Cannot allocate memory in generate_magic");
		exit(1);
	}
	for(int i = 0; i < n ; i++){
		// Used calloc so I can check whether a square has been used already by check its value. If 0, place next number in that spot. If it is any other value, return to original x/y and input value in x - 1.
		*(magic->array + i) = calloc(n, sizeof(int));
		if(*(magic->array + i) == NULL){
			fprintf(stderr, "Cannot allocate memory in generate_magic");
			exit(1);
		}
	}
	// stores previous x and y values
	int x = n / 2;
	int y = 0;
	// stores x and y values if you can succesfully move up and to the right.
	int tempX =0;
	int tempY = 0;
	*(*(magic->array + y) + x) = 1;
	for(int i = 2; i <= (n * n) ; i++){
		tempX = x;
		tempY = y;
		// check if x is on array boundary.
		if(tempX >= n- 1){
			tempX = 0;
		}else{
			tempX += 1;
		}
		// check if y is on array boundary.
		if(tempY <= 0){
			tempY = n - 1;
		}else{
			tempY -= 1;
		}
		// check if a value is in located tempX/TempY already. 
		if(*(*(magic->array + tempY) + tempX) == 0){
			x = tempX;
			y = tempY;
		}else{
			if(y == n - 1){
				y = 0;
			}else{
				y += 1;
			}
		}
		*(*(magic->array + y) + x) = i;
	}	
	return magic;
}

/* write_to_file opens up a new file(or overwrites the existing file)
 * and writes out the square in the format expected by verify_hetero.c
 */
void write_to_file(char *filename, Square * square){
    	// Write the square to file
	FILE * file = fopen(filename, "w");
	if(file == NULL){
		fprintf(stderr, "Could not open file %s.\n", filename);
		exit(1);
	}
	fprintf(file, "%d\n", square->size);
	for(int i = 0; i < square->size; i++){	
		for(int j = 0; j < square->size - 1; j++){
			fprintf(file, "%d,", *(*(square->array + i) + j));
		}
		fprintf(file, "%d\n", *(*(square->array + i) + square->size-1));
	}
	fclose(file);
}
