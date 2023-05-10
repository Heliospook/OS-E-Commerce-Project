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
#include "ux.h"


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

    struct User user = showLogin();
    user = login(user.username, user.password);
    int ret = user.isAdmin;
    showLoginResult(ret, user);
    if(ret < 0) return 0;

    while(1){
        int choice = 5;
        if(ret == 0) choice = 6;
    
        choice = showmenu(ret);
        if(ret == 1){
            if(choice == 1){
                struct Product *pdts = (struct Product *) malloc(256*sizeof(struct Product));
                int n = readProducts(&pdts);

                showproducts(pdts, n);
            }else if(choice == 2){
                struct Product pdt = showCreateProduct();
                int result = createProduct(&pdt);
                showCreateProductResult(result);
            }
            else if(choice == 3){
                int status = 1;
                struct Product pdt = showUpdateProduct();
                status = updateProduct(pdt);
                showUpdateProductResult(status);
            }
            else if(choice == 4){
                int pdtid = showDeleteProduct();                
                int found = deleteProduct(pdtid);
                showDeleteProductResult(found);
            }else if(choice == 5){
                int status = saveuser(user);
                showSaveUserResult(status);
                break;
            }else break;
        }else{
            if(choice == 1){
                struct Product *pdts = (struct Product *) malloc(256*sizeof(struct Product));
                int n = readProducts(&pdts);
                showproducts(pdts, n);
            }else if(choice == 2){
                struct Product pdt = showAddToCart();
                int status = addToCart(&pdt, &user);
                showAddToCartResult(status);
            }else if(choice == 3){
                showCartItems(user.nCart, user.cart);
            }else if(choice == 4){
                struct Product pdt = showRemoveItemsFromCart();
                int status = removeFromCart(&pdt, &user);
                showRemoveItemsFromCartResult(status);
            }else if(choice == 5){
                int ret;
                int nfails = 0;
                struct Product failedcart[10];
                ret = lockCart(user.cart, user.nCart, failedcart, &nfails, semid);
                if(ret){
                    ret = showCheckoutSuccess(user.nCart, user.cart);
                    if(ret){
                        printf("Products were successfully purchased :)\n");
                    }else{
                        printf("Could not complete payment :(.\n");
                    }
                }else{
                    showCheckoutFailure(nfails, failedcart);
                }
            }
            else if(choice == 6){
                int status = saveuser(user);
                showSaveUserResult(status);
                break;
            }else if(choice == 7){
                getsemvals(semid);
            }else break;
        }
    }
    return 0;
}