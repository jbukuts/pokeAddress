// Written by Jake Bukuts 2019
#include <stdio.h>
#include <linux/kernel.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <stdint.h>
#include <iostream>
#include <string>
#include <string.h>
using namespace std;
#include <pthread.h>
#include <vector>

#define VM_READ         0x0001
#define VM_WRITE        0x0002
#define VM_EXEC         0x0004

#define NUM_THREADS 8

#include "main.h"

// get the page size and the address space ranged
unsigned long TASK_SIZE = 0, PAGE_SIZE = 0;

// args for the threads
struct args_struct {
	int id;
	vector<int> flags;
};


int main(int argc, char* argv[])
{
	// set page size and task size
	PAGE_SIZE = sysconf(_SC_PAGESIZE);
	if(sizeof (void*) == sizeof (int)) // 32-bit system
		TASK_SIZE = 0xc0000000UL;
	else // 64-bit system
		TASK_SIZE = (1UL << 47) - PAGE_SIZE;
	
	// see the values
	printf("TASK SIZE IS %lu\n",TASK_SIZE);
	printf("PAGE SIZE IS %lu\n",PAGE_SIZE);
	printf("NEED TO POKE %lu ADDRESSES\n",(TASK_SIZE/PAGE_SIZE));


	// test the hello word call
	long int amma = syscall(436);
	printf("System call sys_hello returned %ld\n", amma);

	// run either the simple or entire test
	// based off of user input
	if (strcmp(argv[1],"base") == 0)
		base_poke();
	else if (strcmp(argv[1],"all") == 0)
		entire_poke();
	else
		printf("Please specift either [base] or [all]\n");

	return 0; 
}

// wrapper for syscall
short poke_addr(void* v) {
	return (short)syscall(437,v);
}

// simple method to print the addr in hex format
void long_to_hex(unsigned long addr) {
	printf("0x%lx\n",addr);
}

// this print the flags
// of given input int
void print_flags(short flags) {
	// initialize as empty
	char read = 'O';
	char write = 'O';
	char exec = 'O';
	
	if (flags == -1) {
		printf("THIS ADDRESS IS INVALID\n");
		return;
	}

	// cast short
	uint8_t f = (uint8_t)flags;

	// check and update flags
	if ((f & VM_READ) > 0)
		read = 'X';
	if ((f & VM_WRITE) > 0)
		write = 'X';
	if ((f & VM_EXEC) > 0)
		exec = 'X';

	// print the values of the flags
	printf("READ:\t%c\nWRITE:\t%c\nEXEC:\t%c\n",read,write,exec);
}

// this does a couple of simple examples
// to check the syscall functionality
int global_variable = 10;
void base_poke() {
	// time to test some simple variables
	int local_variable = 10;
	char * buffer;
	buffer = (char *)malloc (8);
	void* main_point = (void*&)main;
	
	// get the flags for the variables
	short local_flags = poke_addr(&local_variable);
	short global_flags = poke_addr(&global_variable);
	short main_flags = poke_addr(&main_point);
	short buffer_flags = poke_addr(&buffer);

	// print all the flags
	printf("Testing local variable at address");
	printf(" (flags 0x%lx) ",local_flags);
	long_to_hex((unsigned long)&local_variable);
	print_flags(local_flags);

	printf("Testing global variable at address");
	printf(" (flags 0x%lx) ",global_flags);
	long_to_hex((unsigned long)&global_variable);
	print_flags(poke_addr(&global_variable));

	printf("Testing malloc'ed buffer at address");
	printf(" (flags 0x%lx) ",buffer_flags);
	long_to_hex((unsigned long)&buffer);
	print_flags(poke_addr(&buffer));

	printf("Testing main() at address");
	printf(" (flags 0x%lx) ",main_flags);
	long_to_hex((unsigned long)main_point);
	print_flags(poke_addr(&main_point));

	printf("Testing NULL value at address");
	long_to_hex((unsigned long)NULL);
	// printf("NULL poke returns %d\n",(int)poke_addr(NULL));
	print_flags(poke_addr(NULL));

}

// this will go through
// the entire address space
// and poke every address
void entire_poke() {
	// get the page size and the address space ranged
	unsigned long TASK_SIZE = 0, PAGE_SIZE = 0;
	PAGE_SIZE = sysconf(_SC_PAGESIZE);
	if(sizeof (void*) == sizeof (int)) // 32-bit system
		TASK_SIZE = 0xc0000000UL;
	else // 64-bit system
		TASK_SIZE = (1UL << 47) - PAGE_SIZE;

	printf("POKING ENTIRE ADDRESS SPACE!");	

	if (TASK_SIZE % NUM_THREADS == 0)
		printf("FITS NICE");
	else
		printf("NOPE!");


	// start the threads
	int rc;
	pthread_t threads[NUM_THREADS];
	struct args_struct thread_args[NUM_THREADS];
	for (int i=0;i<NUM_THREADS;++i) {
		// set id and start thread
		thread_args[i].id = i;		
		rc = pthread_create(&threads[i],NULL,&thread,(void *)&thread_args[i]);

		// check if thread stared properly
		if (rc) {
			printf("FAILED TO START THREAD!");
			exit(-1);
		}
	}


	// join the threads together
	void *status;
	for (int i=0;i<NUM_THREADS;++i) {
		pthread_join(threads[i],&status);
	}

	// print flags
	for (int i=0;i<NUM_THREADS;++i) {
		for(int j=0;j<thread_args[i].flags.size();++i) {
			print_flags(thread_args[i].flags.at(j));
		}
	}
}

// what each thread will run
void *thread(void *args) {
	// get the page and task size
	unsigned long PAGE_SIZE = 0, TASK_SIZE = 0;
	struct args_struct *a = (struct args_struct *)args;

	PAGE_SIZE = sysconf(_SC_PAGESIZE);
	if(sizeof (void*) == sizeof (int))
		TASK_SIZE = 0xc0000000UL;
	else // 64-bit system
		TASK_SIZE = (1UL << 47) - PAGE_SIZE;

	// get the id of thread
	int id = a->id;
	
	
	// run through loop proper amount
	// but each address poked will be shifted
	// by the offset based of thread id
	unsigned long p = 0;
	unsigned long offset = id*(TASK_SIZE/NUM_THREADS);
	for (; p < (TASK_SIZE/NUM_THREADS); p += (PAGE_SIZE * 1024)) {
		int f = syscall(437,p+offset);
		// printf("POKING ADDRESS %lu\n",(p+offset));
		(a->flags).push_back(f);
	}

	pthread_exit(NULL);
}










