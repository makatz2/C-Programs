////////////////////////////////////////////////////////////////////////////////
// Main File:        division.c
// This File:        division.c
// Other Files:      n/a
// Semester:         CS 354 Spring 2018
//
// Author:           Michael Katz
// Email:            makatz2@wisc.edu
// CS Login:         mkatz@cs.wisc.edu
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
//////////////////////////// 80 columns wide ///////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

static int divCount = 0;

/*  Signal handler for ctrl + c
 *
 * sig     int value of the signal
 * return void
 */
void intHandler(int sig){
     printf("\nTotal number of operations successfully"); 
     printf("completed: %d", divCount);
     printf("\nThe program will be terminated.\n");
     exit(0);
}
/* Signal handler for division by zero
 *
 * sig     int value of the signal
 * return void
 */
void zeroHandler(int sig){
     printf("Error: a division by 0 operation was attempted.\n");
     printf("Total number of operations completed"); 
     printf("successfully: %d", divCount);
     printf("\nThe program will be terminated.\n");
     exit(0);
}
/* Main function that initializes the handlers and sets up simple 
 * division program
 *
 * return 0
 */
int main(){
     struct sigaction saZ;
     memset(&saZ, 0, sizeof(saZ));
     saZ.sa_handler = zeroHandler;
     saZ.sa_flags = 0;

     struct sigaction saI;
     memset(&saI, 0, sizeof(saI));
     saI.sa_handler = intHandler;
     saI.sa_flags = 0;

     if (sigaction(SIGINT, &saI, NULL)){
          printf("error binding to SIGINT handler");
          exit(1);
     }
     if (sigaction(SIGFPE, &saZ, NULL)){
          printf("error binding to SIGFPE handler");
          exit(1);
     }
     while (1){
          char buffer1[100];
          char buffer2[100];
          printf("Enter first integer: ");
          fgets(buffer1, 100, stdin);
          printf("Enter second integer: ");
          fgets(buffer2, 100, stdin);

          int num1 = atoi(buffer1);
          int num2 = atoi(buffer2);
          int div = num1 / num2;
          int rem = num1 % num2;

          printf("%d / %d is %d with a ", num1, num2, div);
          printf("remainder of %d\n", rem);
          divCount++;
     }
     return 0;
}
