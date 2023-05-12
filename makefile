clean : 
	rm server client
	
main :
	gcc -o server backend/server.c
	gcc -o client client.c
	
