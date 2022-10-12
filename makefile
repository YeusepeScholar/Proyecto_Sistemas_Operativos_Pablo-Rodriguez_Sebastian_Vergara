all: cliente gestor

cliente: cliente.o cliente.h fileParser.h extractorConsola.h
	gcc -o cliente cliente.o -lpthread

cliente.o: cliente.c cliente.h fileParser.h extractorConsola.h
	gcc -c cliente.c

gestor: gestor.o fileParser.h extractorConsola.h
	gcc -o gestor gestor.o -lpthread

gestor.o: gestor.c fileParser.h extractorConsola.h
	gcc -c gestor.c

clean:
	rm -f *.o cliente gestor

