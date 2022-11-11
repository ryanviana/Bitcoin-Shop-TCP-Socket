all: mkdir server client 

mkdir: 
		mkdir -p object_files
	
client: client.c
	    gcc -o object_files/client client.c

server: server.c
		gcc -o object_files/server server.c

runserver: server
		./object_files/server 8081

runclient: client
		./object_files/client 127.0.0.1 8081

clear:
		rm -r -f object_files/*
