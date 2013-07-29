CFLAGS  = -I. -Wall -pedantic
LFLAGS  = -L../src
LIBS    = -lgnutls -ltaskd

all: client

clean:
	-rm *.o client

client: client.o
	g++ client.o $(LFLAGS) $(LIBS) -o client

.cpp.o:
	g++ -c $(CFLAGS) $<
