CC = gcc

CFLAGS = -lm -I./include/

OFLAGS = -O2

GFLAGS = -g -Wall

SOURCES = ./src/*.c

all: Cards

Cards: ctags
	$(CC) $(SOURCES) $(CFLAGS) $(GFLAGS) -o Cards 

run:
	./Cards

clean:
	rm Cards

fresh: clean Cards

optimized:
	$(CC) $(SOURCES) $(CFLAGS) $(OFLAGS) -o Cards

ctags: 
	ctags -R *
