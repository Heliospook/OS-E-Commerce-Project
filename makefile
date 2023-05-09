main : 
	gcc -o server backend/server.c
	gcc -o client client.c
	rm client
	rm server
	gcc -o server backend/server.c
	gcc -o client client.c
	