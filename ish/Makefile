CC = gcc

OBJS = lex.yy.o y.tab.o

ish: $(OBJS)
	gcc -g -o $@ $(OBJS) -lfl 

lex.yy.c: ish.l
	lex ish.l
y.tab.c y.tab.h: ish.y
	yacc -d -v ish.y
lex.yy.o : lex.yy.c y.tab.h
	gcc -c -g lex.yy.c  
y.tab.o : y.tab.c 
	gcc -c -g y.tab.c 
clean:
	rm -f lex.yy.c y.tab.c y.tab.h *.o ish y.output
