obj = lex.yy.o parser.o error.o util.o ts.o
CC = gcc
CFLAGS = -g

ucc: $(obj)
	$(CC) -o $@ $(obj) -lm

parser.o: parser.c parser.h codigos.h var_globales.h

lex.yy.o: lex.yy.c codigos.h var_globales.h

error.o: error.c error.h var_globales.h

util.o: util.c util.h var_globales.h

ts.o: ts.c ts.h

clean:
	$(RM) $(obj) ucc.exe