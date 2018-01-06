client.o: client.c
	cc -c client.c

client: client.o
	cc client.o -o client

server2.o: server2.c
		cc -c server2.c
		
server2: server2.o
		cc server2.o -o server -lpthread
