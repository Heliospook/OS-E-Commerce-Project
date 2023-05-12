#include<unistd.h>
#include<stdio.h>
#include<fcntl.h>
#include<sys/stat.h>
#include<stdlib.h>

#include<netinet/in.h>
#include<sys/socket.h>

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

    int num = initsemaphores(semid);
    if(num == -1){
        printf("Couldn't open product file\n");
        return 0;
    }
    printf("%d products found.\n", num);

//-------------server code -------------

    int sfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sfd < 0){
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }
    int opt = 1;
    if(setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))){
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if(bind(sfd, (struct sockaddr *) &address, sizeof(address)) < 0){
        perror("bind");
        exit(EXIT_FAILURE);
    }

    if(listen(sfd, 10) < 0){
        perror("listen");
        exit(EXIT_FAILURE);
    }

    int cfd; int sz = sizeof(address);
    while(1){
        if((cfd = accept(sfd, (struct sockaddr*)&address, (socklen_t *) &sz)) < 0){
            perror("accept");
        }
        if(!fork()){
            struct User user;
            recv(cfd, &user, sizeof(struct User), 0);
            user = login(user.username, user.password);
            int ret = user.isAdmin;
            send(cfd, &ret, sizeof(int), 0);
            if(ret < 0) return 0;

            while(1){
                int choice = 5;
                if(ret == 0) choice = 6;
                recv(cfd, &choice, sizeof(int), 0);
                if(ret == 1){
                    if(choice == 1){
                        struct Product *pdts = (struct Product *) malloc(256*sizeof(struct Product));
                        int n = readProducts(&pdts);
                        send(cfd, &n, sizeof(int), 0);
                        for(int i=0;i<n;i++){
                            send(cfd, pdts + i, sizeof(struct Product), 0);
                        }
                    }else if(choice == 2){
                        struct Product pdt;
                        recv(cfd, &pdt, sizeof(struct Product), 0);
                        int result = createProduct(&pdt, semid);
                        send(cfd, &result, sizeof(int), 0);

                        struct Product *pdts = (struct Product *) malloc(256*sizeof(struct Product));
                        int n = readProducts(&pdts);
                        generateLog(pdts, n, user);
                    }
                    else if(choice == 3){
                        struct Product pdt;
                        recv(cfd, &pdt, sizeof(struct Product), 0);
                        int result = updateProduct(pdt, semid);
                        send(cfd, &result, sizeof(int), 0);

                        struct Product *pdts = (struct Product *) malloc(256*sizeof(struct Product));
                        int n = readProducts(&pdts);
                        generateLog(pdts, n, user);
                    }
                    else if(choice == 4){
                        int pdtid;
                        recv(cfd, &pdtid, sizeof(int), 0);         
                        int result = deleteProduct(pdtid, semid);
                        send(cfd, &result, sizeof(int), 0);

                        struct Product *pdts = (struct Product *) malloc(256*sizeof(struct Product));
                        int n = readProducts(&pdts);
                        generateLog(pdts, n, user);

                    }else if(choice == 5){
                        int status = saveuser(user);
                        send(cfd, &status, sizeof(int), 0);
                        break;
                    }
                }else{
                    if(choice == 1){
                        struct Product *pdts = (struct Product *) malloc(256*sizeof(struct Product));
                        int n = readProducts(&pdts);
                        send(cfd, &n, sizeof(int), 0);
                        for(int i=0;i<n;i++){
                            send(cfd, pdts + i, sizeof(struct Product), 0);
                        }
                    }else if(choice == 2){
                        struct Product pdt;
                        recv(cfd, &pdt, sizeof(struct Product), 0);
                        int result = addToCart(&pdt, &user);
                        send(cfd, &result, sizeof(int), 0);
                    }else if(choice == 3){
                        send(cfd, &user, sizeof(struct User), 0);
                    }else if(choice == 4){
                        struct Product pdt;
                        recv(cfd, &pdt, sizeof(struct Product), 0);
                        int result = removeFromCart(&pdt, &user);
                        send(cfd, &result, sizeof(int), 0);
                    }else if(choice == 5){
                        int ret;
                        int nfails = 0;
                        struct Product failedcart[10];
                        ret = lockCart(user.cart, user.nCart, failedcart, &nfails, semid);
                        if(user.nCart <= 0) ret = 0;
                        send(cfd, &ret, sizeof(int), 0);
                        if(ret){
                            send(cfd, &user, sizeof(struct User), 0);
                            recv(cfd, &ret, sizeof(int), 0);
                            if(ret){
                                purchaseAll(user.cart, user.nCart);
                                generateReceipt(user.cart, user.nCart, user);
                                user.nCart = 0;
                            }else{
                                unlockCart(user.cart, user.nCart, semid);
                            }
                        }else{
                            send(cfd, &nfails, sizeof(int), 0);
                            for(int i=0;i<nfails;i++){
                                send(cfd, failedcart, sizeof(struct Product), 0);
                            }
                        }
                    }
                    else if(choice == 6){
                        int status = saveuser(user);
                        send(cfd, &status, sizeof(int), 0);
                        break;
                    }else if(choice == 7){
                        getsemvals(semid);
                    }
                }
            }
        }else continue;
    }
    shutdown(sfd, SHUT_RDWR);
    return 0;
}