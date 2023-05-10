#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>

#include "backend/src/headers.h"
#include "ux.h"

int main(){
    int cfd = socket(AF_INET, SOCK_STREAM, 0);
    if(cfd < 0){
        printf("Socket creation error.\n");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_port = htons(PORT);
    if(inet_pton(AF_INET, "127.0.0.1", &address.sin_addr) <= 0){
        printf("Invalid address\n");
        exit(EXIT_FAILURE);
    }

    int status = connect(cfd, (struct sockaddr *)&address, sizeof(address));
    if(status < 0){
        printf("Connection failed\n");
        exit(EXIT_FAILURE);
    }

    struct User user = showLogin();
    send(cfd, &user, sizeof(struct User), 0);
    int ret = -1;
    recv(cfd, &ret, sizeof(int), 0);
    showLoginResult(ret, user);
    
    if(ret >= 0){
        while(1){
            int choice = showmenu(ret);
            send(cfd, &choice, sizeof(int), 0);
            if(ret == 1){
                if(choice == 1){
                    struct Product *pdts = (struct Product *) malloc(256*sizeof(struct Product));
                    int n = 0;
                    recv(cfd, &n, sizeof(int), 0);
                    for(int i=0;i<n;i++){
                        recv(cfd, pdts + i, sizeof(struct Product), 0);
                    } 
                    showproducts(pdts, n);
                }else if(choice == 2){
                    struct Product pdt = showCreateProduct();
                    send(cfd, &pdt, sizeof(struct Product), 0);
                    int result = 0;
                    recv(cfd, &result, sizeof(int), 0);
                    showCreateProductResult(result);
                }else if(choice == 3){
                    struct Product pdt = showUpdateProduct();
                    send(cfd, &pdt, sizeof(struct Product), 0);
                    int result = 0;
                    recv(cfd, &result, sizeof(int), 0);
                    showUpdateProductResult(result);
                }else if(choice == 4){
                    int pdtid = showDeleteProduct();
                    send(cfd, &pdtid, sizeof(int), 0);
                    int result = 0;
                    recv(cfd, &result, sizeof(int), 0);
                    showDeleteProductResult(result);
                }else if(choice == 5){
                    int status;
                    recv(cfd, &status, sizeof(int), 0);
                    showSaveUserResult(status);
                }
            }else{
                if(choice == 1){
                    struct Product *pdts = (struct Product *) malloc(256*sizeof(struct Product));
                    int n = 0;
                    recv(cfd, &n, sizeof(int), 0);
                    for(int i=0;i<n;i++){
                        recv(cfd, pdts + i, sizeof(struct Product), 0);
                    } 
                    showproducts(pdts, n);
                }else if(choice == 2){
                    struct Product pdt = showAddToCart();
                    send(cfd, &pdt, sizeof(struct Product), 0);
                    int result = 0;
                    recv(cfd, &result, sizeof(int), 0);
                    showAddToCartResult(result);
                }else if(choice == 3){
                    struct User userdeets;
                    recv(cfd, &userdeets, sizeof(struct User), 0);
                    showCartItems(userdeets.nCart, userdeets.cart);
                }else if(choice == 4){
                    struct Product pdt = showRemoveItemsFromCart();
                    send(cfd, &pdt, sizeof(struct Product), 0);
                    int result = 0;
                    recv(cfd, &result, sizeof(int), 0);
                    showRemoveItemsFromCartResult(result);
                }else if(choice == 5){
                    int ret;
                    recv(cfd, &ret, sizeof(int), 0);
                    if(ret){
                        struct User userdeets;
                        recv(cfd, &userdeets, sizeof(struct User), 0);
                        ret = showCheckoutSuccess(userdeets.nCart, userdeets.cart);
                        send(cfd, &ret, sizeof(int), 0);
                        if(ret){
                            printf("Products were successfully purchased :)\n");
                        }else{
                            printf("Could not complete payment :(.\n");
                        }
                    }else{
                        int nfails = 0;
                        struct Product failedcart[10];
                        recv(cfd, &nfails, sizeof(int), 0);
                        for(int i=0;i<nfails;i++){
                            recv(cfd, failedcart + i, sizeof(struct Product), 0);
                        }
                        showCheckoutFailure(nfails, failedcart);
                    }
                }else if(choice == 6){
                    int status;
                    recv(cfd, &status, sizeof(int), 0);
                    showSaveUserResult(status);
                }
            }
        }
    }
    close(cfd);
    return 0;
}