# makefile de senku // Le Kiwi
PROG = senku

CC = gcc
# gcc
#pour gcc
CFLAGS =  -Wno-unused-parameter -Wno-unused-but-set-variable -Wno-unused-function -Wno-char-subscripts -O3 -Wall -Wextra -o

LDFLAGS= # -mwindows

OBJECTS =   main.o board.o matrix.o peg.o timer.o score.o memento.o

PKG_CONFIG = `pkg-config --cflags --libs gtk+-3.0`

install: all

all: $(PROG)

$(PROG): $(OBJECTS)
	$(CC) $(CFLAGS) $(PROG) $(OBJECTS) $(PKG_CONFIG) $(LDFLAGS)

board.o: board.c  board.h #timer.h score.h
	$(CC) $(CFLAGS) board.o -c board.c $(PKG_CONFIG)

main.o: main.c # board.h # matrix.h
	$(CC) $(CFLAGS) main.o -c main.c $(PKG_CONFIG)

matrix.o: matrix.c matrix.h #peg.h memento.h
	$(CC) $(CFLAGS) matrix.o -c matrix.c $(PKG_CONFIG)

peg.o: peg.c peg.h
	$(CC) $(CFLAGS) peg.o -c peg.c

score.o: score.c score.h
	$(CC) $(CFLAGS) score.o -c score.c

timer.o: timer.c timer.h
	$(CC) $(CFLAGS) timer.o -c timer.c

memento.o: memento.c memento.h #peg.h
	$(CC) $(CFLAGS) memento.o -c memento.c

.PHONY: clean
clean:
	rm *.o
