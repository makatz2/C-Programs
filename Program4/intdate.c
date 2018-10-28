////////////////////////////////////////////////////////////////////////////////
// Main File:        intdate.c
// This File:        intdate.c
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
#include <unistd.h>
#include <time.h>
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>

static int usr1Count = 0;

/*  Signal handler to handle an alarm every 3 seconds
 *
 * sig     int value of the signal
 * return void
 */
void alrmHandler(int sig){
     pid_t pid = getpid();
     time_t t;
     time(&t);
     printf("PID: %d | Current Time: %s", pid, ctime(&t)); 
     alarm(3);
}
/*  Signal handler for a user defined signal
 *
 * sig     int value of the signal
 * return void
 */
void usr1Handler(int sig){
     usr1Count++;
     printf("SIGUSR1 caught!\n");
}
/*  Signal handler for ctrl + c
 *
 * sig     int value of the signal
 * return void
 */
void intHandler(int sig){
     printf("\nSIGINT received.\nSIGUSR1 was received %d times.", usr1Count);
     printf("Exiting now.\n");
     exit(0); 
}
/*  Main function to initialize signal handlers and start 3 second alarm
 *
 * return 0
 */
int main(){
     printf("Pid and time will be printed every 3 seconds.\n");
     printf("Enter ^C to end the program.\n");
     struct sigaction saA;
     memset(&saA, 0, sizeof(saA)); 
     saA.sa_handler = alrmHandler;
     saA.sa_flags = 0;

     struct sigaction saU;
     memset(&saU, 0, sizeof(saU)); 
     saU.sa_handler = usr1Handler;
     saU.sa_flags = 0;

     struct sigaction saI;
     memset(&saI, 0, sizeof(saI)); 
     saI.sa_handler = intHandler;
     saI.sa_flags = 0;
     if (sigaction(SIGALRM, &saA, NULL) != 0){
          printf("error binding to SIGALARM Handler");
          exit(1);
     }
     if (sigaction(SIGUSR1, &saU, NULL) != 0){
          printf("error binding to SIGUSR1 Handler");
          exit(1);
     }
     if (sigaction(SIGINT, &saI, NULL) != 0){
          printf("error binding to SIGINT Handler");
          exit(1);
     }
     alarm(3);     

     while (1){
     }
     return 0;
}
