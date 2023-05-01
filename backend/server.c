#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<unistd.h>
#include<stdio.h>
#include<fcntl.h>
#include<sys/stat.h>
#include<stdlib.h>

#include "./src/headers.h"
#include "./src/utilities.h"

int main(){
//------------initializing semaphores---------
    key_t key = ftok(".", 'a');
    int semid = semget(key, 256, 0777 | IPC_CREAT);
    if(semid == -1){
        printf("Semaphore set creation failed\n");
        return 0;
    }

    int n = initsemaphores(semid);
    if(n == -1){
        printf("Couldn't open product file\n");
        return 0;
    }
    printf("%d products found.\n", n);

//-------------server socket code -------------

    int sd = socket(AF_UNIX,SOCK_STREAM,0);

    struct sockaddr_in server;
    server.sin_family = AF_UNIX;
    server.sin_addr.s_addr = INADDR_ANY; 
    server.sin_port = htonl(5501); 

    bind(sd,(struct sockaddr *) &server, sizeof(server));
    perror("bind");

    listen(sd,10);
    perror("listen");
    while(1){
        struct sockaddr_in client;
        int sz = sizeof(client);
        int newsd = accept(sd, (struct sockaddr *)&client, &sz);

        if(newsd < 0){
            printf("Some error occured!");
            return 0;
        }
        if(!fork()){
            close(sd);
    
            struct User user;
            read(newsd, &user, sizeof(user));
            int ret = login(user.username, user.password);
            write(newsd, &ret, sizeof(ret));
            while(1){
                int choice = 5;
                read(newsd, &choice, sizeof(choice));
                printf("Choice : %d\n", choice);
                if(ret == 1){
                    if(choice == 2){
                        struct Product pdt;
                        read(newsd, &pdt, sizeof(struct Product));
                        int result = createProduct(&pdt);
                        write(newsd, &result, sizeof(int));
                    }
                }else{

                }
                if(choice == 5) break;
            }
            close(newsd);
            return 0;
        }
        else{
            close(newsd);
        }        
    }
    
}