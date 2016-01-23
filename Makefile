
CC = gcc
CFLAGS = -Wall -Wextra -Wpedantic -g -DDEBUG_LEVEL=4
BINDIR = bin/
EXEC = server
SRCDIR = src/

SRCFILES = $(wildcard $(SRCDIR)*.c)


all:
	mkdir -p $(BINDIR)	
	$(CC) $(CFLAGS) -o $(BINDIR)$(EXEC) $(SRCFILES)

run:
	make &&	$(BINDIR)$(EXEC)

clean:
	rm -rf $(BINDIR)
	
