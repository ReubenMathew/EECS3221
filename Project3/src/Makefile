# makefile for Virtual Memeory Unit (MMU)
#
# usage: make mmu 

CC=gcc
CFLAGS=-Wall -W

clean:
	rm -rf *.o
	rm -rf mmu
	
mmu: 
	$(CC) $(CFLAGS) -o mmu mmu.c

test: mmu.c
	./mytest.sh

.PHONY: valgrind
valgrind:
	valgrind --leak-check=full --show-leak-kinds=all ./mmu
	