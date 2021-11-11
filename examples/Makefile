#DEBUG     = -g

CC        = gcc
CPPFLAGS  = $(DEBUG) 
CCFLAGS   = -Wall $(DEBUG) -std=gnu99 -I../src/include/microrl -I../examples -iquote unix_misc
LDFLAGS   = 

TARGET    = microrl_test

all: microrl_test


microrl_test: example.o ../src/microrl/microrl.o  unix_misc/unix_misc.o
	$(CC) $^ -o $@ $(LDFLAGS)

%.o: %.c
	$(CC) -c $< $(CCFLAGS) -o $(*).o

clean:
	rm -f unix_misc/*.o ../src/microrl/*.o *.o $(TARGET)*
