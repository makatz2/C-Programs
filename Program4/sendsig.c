////////////////////////////////////////////////////////////////////////////////
// Main File:        sendsig.c
// This File:        sendsig.c
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
#include <string.h>
#include <signal.h>
#include <stdlib.h>
/*  Main function to send signals to specified process
 *
 *  argc     length of array argv
 *  argv     the command line arguments
 *  return 0
 */
int main(int argc, char* argv[]){
     if (argc != 3){
          printf("Usage: <signal type> <pid>\n");
          return -1;
     }
     if (strcmp("-u", argv[1]) == 0){
          kill(atoi(argv[2]), SIGUSR1);
     } 
     if (strcmp("-i", argv[1]) == 0){
          kill(atoi(argv[2]), SIGINT);
     }
     return 0;
}
