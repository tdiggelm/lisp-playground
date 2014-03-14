FILES = lisp.c
CC = gcc
LDFLAGS = -std=gnu99

lisp: $(FILES)
	$(CC) $(LDFLAGS) -o $@ $^
	
valgrind: lisp
	valgrind --leak-check=full ./lisp
	
.PHONY: clean
clean:
	rm -f lisp *.o
