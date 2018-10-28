////////////////////////////////////////////////////////////////////////////////
// Main File:        verify_hetero.c
// This File:        verify_hetero.c
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
#include <string.h>

// Structure representing Square
// size: dimension(number of rows/columns) of the square
// array: 2D array of integers
typedef struct _Square {
    int size;
    int **array;
} Square;

Square * construct_square(char *filename);
int verify_hetero(Square * square);
void insertion_sort(int* arr, int size);

int main(int argc, char *argv[]){
    	// Check input arguments to get filename
	if(argc != 2){
		fprintf(stderr, "Usage: ./verify_hetero <filename>\n");
		exit(1);
	}
	// Construct square
	Square * Hsquare = construct_square(*(argv + 1));
   	 // Verify if it's a heterosquare and print true or false
	int result = verify_hetero(Hsquare);
	if(result == 0){
		printf("false\n");
	}else{
		printf("true\n");
	}
	// Free up square struct's heap memory
	for(int i = 0; i < Hsquare->size; i++){
		free(*(Hsquare->array + i));
	}
	free(Hsquare->array);	
	free(Hsquare);
    return 0;
}

/* construct_square reads the input file to initialize a square struct
 * from the contents of the file and returns the square.
 * The format of the file is defined in the assignment specifications
 */
Square * construct_square(char *filename){
    // Open file and error check
	FILE * file = fopen(filename, "r");
	if(file == NULL){
		fprintf(stderr, "Can't open input file %s!\n", filename);
		exit(1);
	}
	//create new struct and allocate memory. Error check
	Square * square = malloc(sizeof(Square));
	if(square == NULL){
		fprintf(stderr, "Can't allocate heap memory in construct_square");
		exit(1);
	}
	// Read the first line to get the square size
	int tempSize;
	fscanf(file, "%d", &tempSize);
	// Set struct size and allocate correct memory for array. Error check
	square->size = tempSize;
	square->array = malloc(sizeof(int) * square->size);	
	if(square->array  == NULL){
		fprintf(stderr, "Can't allocate heap memory in construct_square");
		exit(1);
	}
	// Allocate each row in array and input the info from given file
	for(int i = 0; i < square->size; i++){
		*(square->array + i) = malloc(sizeof(int) * square->size);
		if(*(square->array + i)  == NULL){
			fprintf(stderr, "Can't allocate heap memory in construct_square");
			exit(1);
		}
		for(int x = 0; x < square->size; x++){
			int num;	
			fscanf(file, "%d%*c", &num);
			*(*(square->array + i) + x) = num;
		}   
	}
	//close file
	fclose(file);
	return square;
}
/* verify_hetero verifies if the square is a heterosquare
 * 
 * returns 1(true) or 0(false)
 */
int verify_hetero(Square * square)               
{
    // Calculate sum of the following and store it in an array
    // all rows and cols
    // main diagonal
    // secondary diagonal
	// allocate memory to store sums. Error check
	int *temp = malloc(sizeof(int) * ((square->size * 2) + 2) );
	if(temp  == NULL){
		fprintf(stderr, "Can't allocate heap memory in verify_hetero");
		exit(1);
	}
	//Calculate sums for each row and collumn.
	for(int i  = 0; i  < square->size ; i++){
		int row = 0;
		int col = 0;
		for(int j = 0; j < square->size ; j++){
			row += *(*(square->array + i) + j);
			col += *(*(square->array + j) + i);
		}
		*(temp + (2 * i)) = row;
		*(temp + ((2 * i) + 1)) = col;
	}
	//calculate sum of primary and secondary diags
	int primary = 0;
	int secondary = 0;	
	for(int i  = 0; i  < square->size ; i++){
		primary += *(*(square->array + i) + i);
		secondary += *(*(square->array + i) + (square->size - i));
	}
	*(temp +((square->size * 2) + 1)) = primary;
	*(temp +((square->size * 2) + 2)) = secondary; 
   	// Pass the array to insertion_sort function
	insertion_sort(temp, ((square->size * 2) + 2));
  	// Check the sorted array for duplicates
	int result = 1;
	int prev = *temp;
	for(int i = 1 ; i < ((square->size * 2) + 2) ; i++){
		if(prev == *(temp + i)){
			result = 0;
		}else{
			prev =  *(temp + i);
		}
	}
	// Free allocated array.
	free(temp);
    return result;
}

/* insertion_sort sorts the arr in ascending order
 *
 */
void insertion_sort(int* arr, int size)                  
{
    // Sort the arr
	int key;
	for(int i = 2; i < size; i++){
		key = *(arr + i);
		int j = i - 1;
		while( j >= 0 && *(arr + j) > key){
			*(arr + (j + 1)) = *(arr + j);
			j = j - 1;
		}
		*(arr + (j + 1)) = key;
	}
}
