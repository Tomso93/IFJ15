CC=gcc
CFLAGS=-Wall -std=c99 -pedantic -g -lm
OBJECTS=main.o str.o instlist.o interpret.o errors.o ial.o parser.o lex.o 3ac.o varframe.o

all: Project

Project: $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o project

main.o: main.c define.h
	$(CC) $(CFLAGS) -c main.c

str.o: str.c str.h
	$(CC) $(CFLAGS) -c str.c

instlist.o: instlist.c instlist.h
	$(CC) $(CFLAGS) -c instlist.c

interpret.o: interpret.c interpret.h define.h
	$(CC) $(CFLAGS) -c interpret.c

errors.o: errors.c errors.h
	$(CC) $(CFLAGS) -c errors.c

ial.o: ial.c ial.h
	$(CC) $(CFLAGS) -c ial.c

parser.o: parser.c parser.h define.h
	$(CC) $(CFLAGS) -c parser.c

lex.o: lex.c lex.h define.h
	$(CC) $(CFLAGS) -c lex.c

3ac.o: 3ac.c 3ac.h
	$(CC) $(CFLAGS) -c 3ac.c

varframe.o: varframe.c varframe.h
	$(CC) $(CFLAGS) -c varframe.c
