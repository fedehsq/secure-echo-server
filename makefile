CC			=  gcc
AR          =  ar
CFLAGS	    += -std=c99 -Wall
ARFLAGS     =  rvs
INCLUDES	= -I.
LDFLAGS 	= -L.
OPTFLAGS	= -g   #-O3 
LIBS        = -lcrypto

.SUFFIXES: .c .h

%: %.c
	$(CC) $(CFLAGS) $(INCLUDES) $(OPTFLAGS) -o $@ $< $(LDFLAGS) 

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) $(OPTFLAGS) -c -o $@ $<

all: server client clean

run: test 

server: server.o shared.o cryptography.o
	$(CC) $(CCFLAGS) $(INCLUDES) $(OPTFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

client: client.o shared.o cryptography.o
	$(CC) $(CCFLAGS) $(INCLUDES) $(OPTFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

clean: 
	$(RM) sock.socket $(RM) *.o client server

test:
	./server ./client