CC = clang
CFLAGS = -Wall -Wpedantic -Werror -Wextra
LFLAGS = -lm

all: encode decode

encode: encode.o
	$(CC) -o encode encode.o code.o huffman.o io.o node.o pq.o stack.o

encode.o: encode.c code.c huffman.c io.c node.c pq.c stack.c
	$(CC) $(CFLAGS) -c encode.c code.c huffman.c io.c node.c pq.c stack.c

decode: decode.o
	$(CC) -o decode decode.o code.o huffman.o io.o node.o pq.o stack.o

decode.o: decode.c code.c huffman.c io.c node.c pq.c stack.c
	$(CC) $(CFLAGS) -c decode.c code.c huffman.c io.c node.c pq.c stack.c

debug: CFLAGS += -g
debug: all

clean:
	rm -f encode decode encode.o decode.o *.o

format:
	clang-format -i -style=file *.[ch]
