#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<unistd.h>
#include<stdio.h>
#include<fcntl.h>
#include<stdlib.h>
#include<sys/stat.h>

#include"backend/src/headers.h"
#include"ux.h"

int main(){

    int sd = socket(PF_UNIX,SOCK_STREAM,0);
     
    struct sockaddr_in server ;
    server.sin_addr.s_addr = INADDR_ANY;  
    server.sin_family = AF_UNIX;
    server.sin_port = htonl(5500);

    int ret = connect(sd, (struct sockaddr *)&server, sizeof(server));
    perror("connect");
    if(ret == -1){
        printf("Failed to connect.\n");
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
        write(sd, &choice, sizeof(choice));
        if(status == 1){
            if(choice == 1){
                int n = 0;
                read(sd, &n, sizeof(int));
                struct Product *products  = (struct Product *) malloc(n * sizeof(struct Product));
                for(int i=0;i<n;i++){
                    read(sd, products + i, sizeof(struct Product));
                }
                showproducts(products, n);
            }
            else if(choice == 2){
                struct Product pdt = showCreateProduct();
                write(sd, &pdt, sizeof(struct Product));
                int result = 0;
                read(sd, &result, sizeof(result));
                drawline();
                if(result <= 0){
                    printf("Pdt. creation failed.\n");
                }else{
                    printf("Pdt. created successfully with id = %d\n", result - 1);
                }
            }else if(choice == 3){
                struct Product pdt = showUpdateProduct();
                write(sd, &pdt, sizeof(struct Product));
                int result = 1;
                read(sd, &result, sizeof(int));
                if(result){
                    printf("Product updated successfully.\n");
                }else printf("Product updation failed.\n");
            }else if(choice == 4){
                int result = 0, pdtid;
                pdtid = showDeleteProduct();
                write(sd, &pdtid, sizeof(int));
                perror("client write");
                read(sd, &result, sizeof(int));
                perror("client read");
                printf("result : %d\n", result);
                if(result){
                    printf("Product deleted successfully.\n");
                }else printf("Such a product was not found.\n");
            }
        }
        if(choice == 5) break;
    }
    close(sd);
    return 0;
}
