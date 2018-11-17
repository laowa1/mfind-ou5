#COMPILER
CC = gcc

#PATH TO INCLUDE
CFLAGS = -g -I / -pthread # -std=gnu11 -Wall -Wextra -Werror -Wmissing-declarations -Wmissing-prototypes -Werror-implicit-function-declaration -Wreturn-type -Wparentheses -Wunused -Wold-style-definition -Wundef -Wshadow -Wstrict-prototypes -Wswitch-default -Wunreachable-code

#DEPENDENCIES
DEPS = mfind.h stack.h

#FILES TO COMPILE
OBJ = mfind.o stack.o

all: mfind

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

mfind: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

clean:
	rm -f $(OBJ) mfind
