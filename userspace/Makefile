CC = g++  # use g++ compiler

FLAGS = -std=c++11  # compile with C++ 11 standard
FLAGS += -Wall      # compile with all warnings

INCLUDE = -I inc  # add include and test dirs to include path
FLAGS += $(INCLUDE)

LINK = $(CC) $(FLAGS) -o  # final linked build to binary executable

COMPILE = $(CC) $(FLAGS) -c  # compilation to inter	mediary .o files

test: clean build
	./main

build: main

main: main.o
	$(LINK) $@ $^
main.o: main.cc
	$(COMPILE) $<

# clean all build files
clean:
	rm -f main
	rm -f *.o
