#ifndef MAIN_H
#define MAIN_H

int main(int argc, char* argv[]);
void base_poke();
void entire_poke();
short poke_addr(void* v);
void print_flags(short flags);
//void long_to_hex(unsigned long addr);
void *Thread(void *args);
struct args_struct;

#endif
