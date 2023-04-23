#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<unistd.h>
#include<stdio.h>
#include<fcntl.h>
#include<stdlib.h>
#include<sys/stat.h>

#include"backend/headers.h"
#include"ux.h"

int main(){

    int sd = socket(PF_UNIX,SOCK_STREAM,0);
     
    struct sockaddr_in server ;
    server.sin_addr.s_addr = INADDR_ANY;  
    server.sin_family = AF_UNIX;
    server.sin_port = htonl(5050);

    int ret = connect(sd, (struct sockaddr *)&server, sizeof(server));
    if(ret == -1){
        printf("Failed to connect.");
        return 0;
    }

    // ------------------------------Authentication-------------------------------

    struct User user;
    printf("Enter username : \n");
    scanf("%s", user.username);
    printf("Enter password : \n");
    scanf("%s", user.password);

    write(sd, &user, sizeof(user));

    int status;
    read(sd,&status,sizeof(status));
    if(status == 1){
        printf("Logged in as admin %s.\n", user.username);
    }else if(status == 0){
        printf("Logged in as User %s.\n", user.username);
    }else{
        printf("Authentication Failed. Invalid credentials.\n");
        close(sd);
        return 0;
    }
    
    //-------------------------------Menu----------------------------------
    while(1){
        int choice = showmenu(status);
        if(choice == 5) break;
    }
    close(sd);
    return 0;
}
