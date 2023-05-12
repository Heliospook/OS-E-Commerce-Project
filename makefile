clean : 
	rm server client server2
	
main :
	gcc -o server backend/server.c
	gcc -o client client.c
	
