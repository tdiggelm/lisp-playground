all: lisp
	gcc -o lisp lisp.c
	
valgrind: all
	valgrind --leak-check=full ./lisp
	
.PHONY: clean
clean:
	rm lisp
