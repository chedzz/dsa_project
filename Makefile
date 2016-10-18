all: try abc
try: infix.o postfix2.o stack.o cstack.o token.o
	cc infix.o postfix2.o stack.o cstack.o token.o -o try
infix.o: infix.c token.h cstack.h 
	cc -c infix.c
postfix2.o: postfix2.c stack.h  token.h stack.o
	cc -c postfix2.c
stack.o: stack.c stack.h  
	cc -c stack.c
cstack.o: cstack.c cstack.h
	cc -c cstack.c
token.o:  token.c token.h
	cc -c token.c
abc: abc.c stack.h stack.o 
	cc -c abc.c
	cc abc.o stack.o -o abc	
clean: 
	rm *.o
