CFLAGS  = -I. -Wall -pedantic
LFLAGS  = -L../src
LIBS    = -lgnutls -ltaskd

all: client add

clean:
	-rm *.o client

client: client.o
	g++ client.o $(LFLAGS) $(LIBS) -o client

add: add.o
	g++ add.o $(LFLAGS) $(LIBS) -o add

.cpp.o:
	g++ -c $(CFLAGS) $<
