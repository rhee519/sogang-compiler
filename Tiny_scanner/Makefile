#
# makefile for TINY
# Borland C Version
# K. Louden 2/3/98
#

CC = gcc

CFLAGS = -g -Wall

# OBJS = main.o util.o scan.o parse.o symtab.o analyze.o code.o cgen.o
OBJS = main.o util.o lex.yy.o

TARGET = hw1_binary

RFLIST = result_file_list.txt

# -lfl: noyywrap 옵션
# https://stackoverflow.com/questions/1811125/undefined-reference-to-yywrap
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(TARGET) -lfl

# main.o: main.c globals.h util.h scan.h parse.h analyze.h cgen.h
# 	$(CC) $(CFLAGS) -c main.c
main.o: main.c globals.h util.h scan.h
	$(CC) $(CFLAGS) -c main.c

util.o: util.c util.h globals.h
	$(CC) $(CFLAGS) -c util.c

lex.yy.o: lex.yy.c globals.h util.h scan.h
	$(CC) $(CFLAGS) -c lex.yy.c

lex.yy.c: lex/cminus.l
	flex lex/cminus.l

# scan.o: scan.c scan.h util.h globals.h
# 	$(CC) $(CFLAGS) -c scan.c

# parse.o: parse.c parse.h scan.h globals.h util.h
# 	$(CC) $(CFLAGS) -c parse.c

# symtab.o: symtab.c symtab.h
# 	$(CC) $(CFLAGS) -c symtab.c

# analyze.o: analyze.c globals.h symtab.h analyze.h
# 	$(CC) $(CFLAGS) -c analyze.c

# code.o: code.c code.h globals.h
# 	$(CC) $(CFLAGS) -c code.c

# cgen.o: cgen.c globals.h symtab.h code.h cgen.h
# 	$(CC) $(CFLAGS) -c cgen.c

# lex.yy.o: cminus.l scan.h util.h globals.h
# 	flex -o lex.yy.c cminus.l
# 	$(CC) $(CFLAGS) -c lex.yy.c

clean:
	rm -f $(TARGET) $(OBJS) lex.yy.c
	cat ${RFLIST} | xargs rm -f
	rm ${RFLIST}

# tiny: tiny.exe

# tm: tm.exe

# all: tiny tm

all: $(TARGET)

