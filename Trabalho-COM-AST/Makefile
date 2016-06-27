PROG=tp

all: $(PROG)  

$(PROG): gram.c lex.c
	gcc -g -o $(PROG) interpreter.c vc.c lex.c gram.c

gram.c gram.h: $(PROG).y
	bison --debug -g -d -v -o gram.c $(PROG).y 
 
lex.c: $(PROG).l gram.h
	flex -o lex.c $(PROG).l

