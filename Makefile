FILES = scheme.c
CC = gcc
LDFLAGS = -std=gnu99

scheme: $(FILES)
	$(CC) $(LDFLAGS) -o $@ $^
	
valgrind: scheme
	valgrind --leak-check=full ./scheme
	
.PHONY: clean
clean:
	rm -f scheme *.o
	
.PHONY: rebuild
rebuild: clean scheme
