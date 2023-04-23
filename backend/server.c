#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<unistd.h>
#include<stdio.h>
#include<fcntl.h>
#include<sys/stat.h>
#include<stdlib.h>

#include "headers.h"
#include "utilities.h"

int main(){
    int sd = socket(AF_UNIX,SOCK_STREAM,0);

    struct sockaddr_in server;
    server.sin_family = AF_UNIX;
    server.sin_addr.s_addr = INADDR_ANY; 
    server.sin_port = htonl(5500); 

    bind(sd,(struct sockaddr *) &server, sizeof(server));

    listen(sd,10);
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
            
            close(newsd);
            return 0;
        }
        else{
            close(newsd);
        }        
    }
    
}