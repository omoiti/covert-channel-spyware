/*
*Code for spy program
*The spy program is a program written  to measure execution time of instructions altered by trojan
*The trojan (program) affects the branch prediction accuracy of the spy (program) (newly scheduled process)
The results are stored in the file output.csv
*/

#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <time.h>
#include <stdint.h>
#include <stddef.h>  
#include <stdlib.h>  
#include <stdbool.h>
#include <signal.h>  
#include "CircularArray.c"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>


#define PATH_NAME "pipe"

#define MAX_PROBES 200
#define SLEEP_T 20

extern void branch();

uint64_t rdtsc(){		//This here defines the rdtsc function, which returns system time.
    uint32_t lo, hi;
    __asm__ __volatile__ (
                          "xorl %%eax,%%eax \n        cpuid"
                          ::: "%rax", "%rbx", "%rcx", "%rdx");
    __asm__ __volatile__ ("rdtsc" : "=a" (lo), "=d" (hi));
    return (uint64_t)hi << 32 | lo;
}

void bptSpy(int timings[MAX_PROBES]){
//bptSpy probes the BPT and returns an array of timings.
	uint64_t start_t=0, end_t=0;
	int i; 
	//MAX_PROBES is the number of timings to take
	for(i = 0; i < MAX_PROBES; ++i){
		//Yield the processor for some time
		usleep(SLEEP_T); 
		start_t=rdtsc(); //Start timing
		//Do a whole bunch of attempted branches
		branch();		 
		end_t=rdtsc();	 //Stop timing!
		//See how long it took, put in array.
		timings[i] = end_t - start_t;	
	}
	return;
}

bool keepRunning = true;

int intHandler(int signal){
//intHandler allows the user the CTRL^C out of the spy. 
	keepRunning = false;
	return 0;
}

int average(int timings[MAX_PROBES]){
//Find the average of the integers in the array.
	int i = 0;
	int accumulator = 0;
	for (i = 0; i < MAX_PROBES; ++i){
		accumulator += timings[i];
	}
	return accumulator/MAX_PROBES;
}

void restart(){
//restart() exits the spy, so that the filter may run.
	exit(42);
}

void main(int argc, char* argv[]){
	//Set up a pipe to allow use to CTRL^c from the spy.  
	struct sigaction act;
    act.sa_handler = (void*)(intHandler);
    sigaction(SIGINT, &act, NULL);

	FILE* output;
	
	//Declare an array to store timings from BPT
	int timings[MAX_PROBES];

	//The circular array is used to shifted average deviation calculations
	CircularArray buf;
	//Call the CircularArray factory function
	init(&buf);
	int avg = 0;
	int j = 0;
	int noiseCounter = 0;
	int dataCounter = 0;

 	noiseCounter = 0;
 	
 	output = fopen("output.csv", "w");

 	//Declare time objects to measure the duration of transmission.
	time_t start = time(NULL);
	time_t end;

	//The noiseCounter value is incremented each time the data in the 
	// buffer is regarded to be noisy. It is reset to 0 if that changes.
	while(noiseCounter <= 350){
		//Call the spy routine
		bptSpy(timings);   	          
   		//Get the average timings
   		avg = average(timings);
   		//Update the data counter
   		++dataCounter;
   		//Add the timing to the buffer
   		shift(&buf, avg);
   		//Print out the results	   	
	   	fprintf(output, "%d\n", avg);	   	
	   	//Detect noise on the channel, either through a high deviation, 
	   	// or if the average of the readings in the buffer is high.
	   	//Noise on the channel tends to result in readings much higher than 
	   	// either low or high bit transmissions. 
	   	if(averageDev(&buf) >= 15 || buf.sum/buf.size > 250 ) noiseCounter+=1;
	   	else noiseCounter = 0;
    }
	    
    end = time(NULL);

    double duration = difftime(end, start);

    //If the duration of reading is long enough for a character to have been read,
    // print the transmission time.
    if (duration >= 20.0) printf("Transmission took %f seconds. \n", duration); 
    
    //Exit the spy, so the filter may be called.
    restart();
}

