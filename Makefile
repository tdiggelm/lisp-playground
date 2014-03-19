FILES = scheme.c
CC = gcc
LDFLAGS = -std=gnu99

all: scheme test-gc

scheme: $(FILES)
	$(CC) $(LDFLAGS) -o $@ $^
	
valgrind: scheme
	valgrind --leak-check=full ./scheme
	
test-gc: test-gc.c
	gcc -o test-gc test-gc.c -lgc
	
.PHONY: clean
clean:
	rm -f "test-gc" scheme *.o
	
.PHONY: rebuild
rebuild: clean scheme
