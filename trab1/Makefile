CFLAGS = -lm -Wall
LDFLAGS = -lm
OBJSS = servidor.o rawsocket.o interface.o
OBJSC = cliente.o rawsocket.o interface.o

all: servidor cliente
server: servidor
client: cliente

cliente: $(OBJSC)
	gcc -o cliente $(CFLAGS) $(OBJSC) $(LDFLAGS)

servidor: $(OBJSS)
	gcc -o servidor $(CFLAGS) $(OBJSS) $(LDFLAGS)

rawsocket.o: rawsocket.c
	gcc $(CFLAGS) -c rawsocket.c $(LDFLAGS)

servidor.o: servidor.c
	gcc $(CFLAGS) -c servidor.c $(LDFLAGS)

cliente.o: cliente.c
	gcc $(CFLAGS) -c cliente.c $(LDFLAGS)
interface.i: interface.clean
	gcc $(CFLAGS) -c interface.c $(LDFLAGS)

clean:
	-rm -f *~ *.o

purge: clean
	-rm servidor cliente