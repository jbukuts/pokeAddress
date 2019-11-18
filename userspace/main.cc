// Written by Jake Bukuts 2019
#include <stdio.h>
#include <linux/kernel.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <iostream>
#include <string>
#include <string.h>
using namespace std;

#define VM_READ         0x0001
#define VM_WRITE        0x0002
#define VM_EXEC         0x0004

#include "main.h"

int main(int argc, char* argv[])
{
	// test the hello word call
	long int amma = syscall(335);
	printf("System call sys_hello returned %ld\n", amma);

	// run either the simple or entire test
	// based off of user input
	if (strcmp(argv[1],"base") == 0)
		base_poke();
	else if (strcmp(argv[1],"all") == 0)
		entire_poke();
	else
		printf("Please specift either [base] or [all]");

	return 0; 

}

// wrapper for syscall
int poke_addr(void* v) {
	return syscall(336,v);
}

// this print the flags
// of given input int
void print_flags(int flags) {
	// initialize as empty
	char read = 'O';
	char write = 'O';
	char exec = 'O';

	// check and update flags
	if ((flags & VM_READ) > 0)
		read = 'X';
	if ((flags & VM_WRITE) > 0)
		write = 'X';
	if ((flags & VM_EXEC) > 0)
		exec = 'X';

	// print the values
	printf("\nREAD:\t%c\nWRITE:\t%c\nEXEC:\t%c\n",read,write,exec);
}

// this does a couple of simple examples
// to check the syscall functionality
int global_variable = 10;
void base_poke() {
	// time to test some simple variables
	int local_variable = 10;

	printf("Testing local variable:");
	print_flags(poke_addr(&local_variable));

	printf("Testing global variable:");
	print_flags(poke_addr(&global_variable));
	
	printf("Testing NULL value");
	print_flags(poke_addr(NULL));

}

// this will go through
// the entire address space
// and poke every address
void entire_poke() {
	// get the page size and the address space ranged
	unsigned long PAGE_SIZE = 0, TASK_SIZE = 0;
	PAGE_SIZE = sysconf(_SC_PAGESIZE);
	if(sizeof (void*) == sizeof (int)) // 32-bit system
		TASK_SIZE = 0xc0000000UL;
	else // 64-bit system
		TASK_SIZE = (1UL << 47) - PAGE_SIZE;
	  
	// go through entire address range
	char *p = 0;
	unsigned long validAddr = 0, invalidAddr = 0;
	for( ; (unsigned long)p < TASK_SIZE; p += PAGE_SIZE * 1024 ) {
	 	// get the flags
		int r = syscall(336,p);
		// print the flags out
		printf("FLAGS AT ADDRESS %lu",(unsigned long)p);
		print_flags(r);		
	}
}








