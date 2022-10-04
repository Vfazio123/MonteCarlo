#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>  
#include <sys/time.h>
/* Program to compute Pi using Monte Carlo methods */
/*
   To compile:
      gcc -o MonteCarlo_v2 MonteCarlo_v2.c -pthreads
   To run:
      ./MonteCarlo_v2 10 2
        argv[1] is # of iterations
        argv[2] is # of threads
*/
/*error handling function: prints out error message*/
int print_error(char * msg) 
{
   fprintf(stderr, "%s\n", msg);
   exit(2);
}

//Delcaring a struct to pass IN/OUT points back to main thread
struct IN_OUT_INFO {
   int IN; //# of points inside the circle
   int OUT; //# of points outside the circle
   int Number_of_points;
   int nthreads;
};

/* The "thread function" passed to pthread_create.  Each thread executes this
 * function and terminates when it returns from this function. */
void * MonteCarlo(void * Number_of_Point) 
{
   struct IN_OUT_INFO *rand_Pi = (struct IN_OUT_INFO *)Number_of_Point;
   int i=0; //Control loop variable
   double x;
   double y;

   /* Intializes random number generator */
   srand((unsigned) time(NULL));

   /* Print Number_of_points_int random numbers from 0 to 1 */
   for(i=0; i < (rand_Pi->Number_of_points/rand_Pi->nthreads); i++)  {
      //Randomly assigning x,y a number 0 to 1
      x = rand()%100;
      x /= 100;
      y = rand()%100;
      y /= 100;
      
      //Making some x,y numbers negative to get the other half of the circle/squre
      if(rand()%2){ 
         x *= -1;
      }
		if(rand()%2) {
         y *= -1;
      }
      
      //Determine if the point lies within the circle or not
      //x^2+y^=r^2 - <=1 when r is equal to 1
      if ((pow(x,2) + pow(y,2)) <= 1){
         rand_Pi->IN++;
      } else {
         rand_Pi->OUT++;
      }
   }

   return 0;
}

int main(int argc, char** argv) 
{
   struct IN_OUT_INFO rand_Pi;
   int i = 0;
   int ret; //useful for error detection
   double PI = 0; //variable that holds PI   
   
   /* Read the number of threads to create from the command line. */
   if (argc !=3) {
      fprintf(stderr, "usage: %s <n>\n", argv[0]);
      fprintf(stderr, "where <n> is the number of threads\n");
      return 1;
   }

   
   rand_Pi.Number_of_points = atoi(argv[1]);
   rand_Pi.nthreads = atoi(argv[2]);
   rand_Pi.IN = 0;
   rand_Pi.OUT = 0;
   

   pthread_t thread_array[rand_Pi.nthreads]; //pointer to future thread array

   if (rand_Pi.nthreads < 1) print_error("ERROR: enter a positive number of threads");

   //https://linuxhint.com/gettimeofday_c_language/
   //timeval is a struct in sys.time.h that holds 
   //tv_sec : It is the number of seconds since the epoch 
   //tv_usec: It is additional microseconds after number of seconds calculation since the epoch. 
   struct timeval start, end;
   gettimeofday(&start, NULL);

   /* Assign each thread an ID and create all the threads. */
   for (i=0; i < rand_Pi.nthreads; i++) {
      ret = pthread_create(&thread_array[i], NULL, MonteCarlo, (void *) &rand_Pi);
      if (ret) print_error("ERROR: pthread create failed");
   }


   /* Join all the threads. Main will pause in this loop until all the threads
    * have returned from the thread function. */
   for (i = 0; i < rand_Pi.nthreads; i++) {
      ret = pthread_join(thread_array[i], NULL);
      if (ret) print_error("ERROR: pthread join failed");
   }

   //Tracking the amount of time to execute
   gettimeofday(&end, NULL);
   printf("Time for program to run took %ld micro seconds\n",((end.tv_sec * 1000000 + end.tv_usec) - (start.tv_sec * 1000000 + start.tv_usec)));

   PI = (double)4*rand_Pi.IN/(rand_Pi.IN+rand_Pi.OUT);
  //printf("IN: %i\n",rand_Pi.IN);
   //printf("OUT: %i\n",rand_Pi.OUT);
   printf("PI: %f\n",PI);

   return 0;
}
