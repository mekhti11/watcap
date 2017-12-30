client.o: client.c
	cc -c client.c 
server.o: server.c
	cc -c server.c 

client: client.o 
	cc client.o -o client
server: server.o
	cc server.o -o server