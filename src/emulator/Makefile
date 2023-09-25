CC	= gcc
CFLAGS	= -Wall -g -D_POSIX_SOURCE -D_DEFAULT_SOURCE -std=c99 -pedantic	

.SUFFIXES: .c .o

all: emulate.o branch.o data_processing.o data_transfer.o utils.o 
	$(CC) emulate.o branch.o data_processing.o data_transfer.o utils.o -o ../emulate

emulate.o: emulate.c
	$(CC) $(CFLAGS) emulate.c -c -o emulate.o

branch.o: branch.c
	$(CC) $(CFLAGS) branch.c -c -o branch.o

data_processing.o: data_processing.c
	$(CC) $(CFLAGS) data_processing.c -c -o data_processing.o

data_transfer.o: data_transfer.c
	$(CC) $(CFLAGS) data_transfer.c -c -o data_transfer.o

utils.o: utils.c
	$(CC) $(CFLAGS) utils.c -c -o utils.o

clean:
	-rm *.o ../emulate