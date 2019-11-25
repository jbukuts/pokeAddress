// Written by Jake Bukuts 2019
#include <stdio.h>
#include <linux/kernel.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <stdint.h>
#include <iostream>
#include <string>
#include <string.h>
#include <pthread.h>
#include <vector>
#include <stack>
#include <iostream>
using namespace std;

#define VM_READ         0x0001
#define VM_WRITE        0x0002
#define VM_EXEC         0x0004

#define KNRM  	"\033[0m"
#define KRED  	"\033[31m"
#define KGRN  	"\033[32m"
#define KBOLD	"\e[1m"

#define NUM_THREADS 4

#include "userspace.h"

// get the page size and the address space ranged
unsigned long TASK_SIZE = 0, PAGE_SIZE = 0;
const int g = 0;
unsigned long validAddr = 0;
unsigned long invalidAddr = 0;

struct args_struct {
	int id;
	stack<int> flags;
};

int main(int argc, char* argv[])
{
	// get the page size
	PAGE_SIZE = sysconf(_SC_PAGESIZE);
	if(sizeof (void*) == sizeof (int)) // 32-bit system
		TASK_SIZE = 0xc0000000UL;
	else // 64-bit system
		TASK_SIZE = (1UL << 47) - PAGE_SIZE;
	
	setlocale(LC_NUMERIC, "");
	printf("TASK SIZE IS %'lu!\n",TASK_SIZE);
	printf("PAGE SIZE IS %'lu!\n",PAGE_SIZE);
	printf("THAT MAKES %'lu ADDRESSES TO LOOK AT!\n",TASK_SIZE/(PAGE_SIZE*1024));

	// test the hello word call
	long int amma = syscall(335);
	printf("System call sys_hello returned %ld\n\n", amma);

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
	return (short)syscall(336,v);
}

// this print the flags
// of given input int
void print_flags(short flags) {
	// initialize as no
	string red = (string)KRED;
	string norm = (string)KNRM;
	string green = (string)KGRN;
	
	string read = red+"N"+norm;
	string write = red+"N"+norm;
	string exec = red+"N"+norm;
	
	if (flags == -1) {
		printf("THIS ADDRESS IS INVALID\n");
		return;
	}
	uint8_t f = (uint8_t)flags;

	// check and update flags
	if ((f & VM_READ) > 0)
		read = green+"Y"+norm;
	if ((f & VM_WRITE) > 0)
		write = green+"Y"+norm;
	if ((f & VM_EXEC) > 0)
		exec = green+"Y"+norm;

	// print the values
	cout << "READ:\t" << read << endl;
	cout << "WRITE:\t" << write << endl;
	cout << "EXEC:\t" << exec << endl;
}

// this does a couple of simple examples
// to check the syscall functionality
void base_poke() {
	// time to test some simple variables
	int local_variable = 10;
	char * buffer;
	buffer = (char *)malloc (8);

	// get the flags
	int local_flags = poke_addr((void *)&local_variable);
	int global_flags = poke_addr((void *)&g);
	int main_flags = poke_addr(&(void*&)main);
	int buffer_flags = poke_addr((void *)&buffer);
	int null_flags = poke_addr(NULL);

	// print the flags
	printf("Testing local variable at address 0x%lx (%d)\n",(unsigned long)&local_variable,local_flags);
	print_flags(local_flags);

	printf("Testing global variable at address 0x%lx (%d)\n",(unsigned long)&g,global_flags);
	print_flags(global_flags);

	printf("Testing malloc'ed buffer at address 0x%lx (%d)\n",(unsigned long)&buffer,buffer_flags);
	print_flags(buffer_flags);

	printf("Testing main() at address 0x%lx (%d)\n",(unsigned long)(void*&)main,main_flags);
	print_flags(main_flags);

	printf("Testing NULL value at address 0x%lx (%d)\n",(unsigned long)NULL,null_flags);
	print_flags(null_flags);

	printf("DONE!\n");

}

// this will go through TASK_SIZE / (PAGE_SIZE*1024)
void entire_poke() {

	char *p = 0;
	for (; (unsigned long)p < TASK_SIZE; p += (PAGE_SIZE * 1024)) {
		int f = syscall(336,p);
		if (f < 0)
			++invalidAddr;
		else {
			++validAddr;
			printf("VALID ADDRESS AT 0x%lx WITH FLAGS (%d)\n",(unsigned long)p,f);
			print_flags(f);
		}
	}
	// print the final results
	printf("\nVALID ADDRESSES:\t%'lu\nINVALID ADDRESSES:\t%'lu\n",validAddr,invalidAddr);
}

void *Thread(void *args) {
	struct args_struct *a = (struct args_struct *)args;
	int id = a->id;
	
	// find the starting address
	char *p = 0;
	char *offset = (char*)(id*(TASK_SIZE/NUM_THREADS));
	printf("THREAD %d will run from %lu TO %lu\n",id,(unsigned long)offset,(unsigned long)offset+(TASK_SIZE/NUM_THREADS)-1);
	for (; (unsigned long)p < (TASK_SIZE/NUM_THREADS); p += (PAGE_SIZE * 1024)) {
		int f = syscall(336,p+(unsigned long)offset);
		
		if (f < 0)
			++invalidAddr;
		else
			++validAddr;

		// printf("[%d]POKING ADDRESS %lx FLAGS ARE %d\n",id,(unsigned long)(p+(unsigned long)offset),f);
		(a->flags).push(f);
	}

	printf("THREAD %d COMPLETED!\n",id);
	pthread_exit(NULL);
}
