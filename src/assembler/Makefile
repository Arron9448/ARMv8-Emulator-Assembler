CC	= gcc
CFLAGS	= -Wall -g -D_POSIX_SOURCE -D_DEFAULT_SOURCE -std=c99 -pedantic	

.SUFFIXES: .c .o

all: assemble.o branch.o data_processing.o data_transfer.o tokenize.o utils.o symbol_table.o
	$(CC) assemble.o branch.o data_processing.o data_transfer.o tokenize.o utils.o symbol_table.o -o ../assemble

assemble.o: assemble.c
	$(CC) $(CFLAGS) assemble.c -c -o assemble.o

branch.o: branch.c
	$(CC) $(CFLAGS) branch.c -c -o branch.o

data_processing.o: data_processing.c
	$(CC) $(CFLAGS) data_processing.c -c -o data_processing.o

data_transfer.o: data_transfer.c
	$(CC) $(CFLAGS) data_transfer.c -c -o data_transfer.o

tokenize.o: tokenize.c
	$(CC) $(CFLAGS) tokenize.c -c -o tokenize.o

utils.o: utils.c
	$(CC) $(CFLAGS) utils.c -c -o utils.o

symbol_table.o: symbol_table.c
	$(CC) $(CFLAGS) symbol_table.c -c -o symbol_table.o

clean:
	-rm *.o ../assemble