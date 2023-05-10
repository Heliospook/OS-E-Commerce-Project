#include <stdio.h>
#include"backend/src/headers.h"
#include<string.h>
#ifndef UX
#define UX

int drawline(){
    for(int i=0;i<30;i++) printf("-");
    printf("\n");
}

int showmenu(int status){
    if(status == 1){
        drawline();
        printf("Admin Panel \n");
        drawline();
        printf("1) View all products\n");
        printf("2) Add a product\n");
        printf("3) Modify a product\n");
        printf("4) Delete a product\n");
    }else{
        drawline();
        printf("User Panel \n");
        drawline();
        printf("1) View all products\n");
        printf("2) Add product to cart\n");
        printf("3) View cart\n");
        printf("4) Checkout\n");
    }
    printf("5) Exit\n");
    drawline();
    int choice;
    scanf("%d", &choice);
    return choice;
}

struct Product showCreateProduct(){
    struct Product pdt;
    printf("Enter product name : \n");
    scanf("\n%[^\n]s", pdt.name);
    printf("Enter product quantity : \n");
    scanf("%d", &pdt.quantity);
    printf("Enter product price : \n");
    scanf("%lf", &pdt.price);
    return pdt;
}

void showproducts(struct Product *products, int n){
    drawline();
    printf("Here is the list of all available products : \n");
    drawline();
    for(int i=0;i<n;i++){
        printf("%d) %s : %d left - %0.2lf inr\n", products[i].id, products[i].name, products[i].quantity, products[i].price);
    }
    drawline();
}

struct Product showUpdateProduct(){
    struct Product pdt;
    printf("Enter the id of the product to be updated : \n");
    scanf("%d", &pdt.id);
    printf("Enter product name : \n");
    scanf("\n%[^\n]s", pdt.name);
    printf("Enter product quantity : \n");
    scanf("%d", &pdt.quantity);
    printf("Enter product price : \n");
    scanf("%lf", &pdt.price);
    return pdt;
}

int showDeleteProduct(){
    int pdtid = -1;
    printf("Enter the id of the product to be deleted : \n");
    scanf("%d", &pdtid);
    return pdtid;
}

 

#endif