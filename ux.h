#include <stdio.h>
#include"backend/src/headers.h"
#include<string.h>
#ifndef UX
#define UX

int drawline(){
    for(int i=0;i<30;i++) printf("-");
    printf("\n");
}
struct User showLogin(){
    struct User user;
    printf("Enter username : \n");
    scanf("%s", user.username);
    printf("Enter password : \n");
    scanf("%s", user.password);
    return user;
}

void showLoginResult(int status, struct User user){
    if(status == 1){
        printf("Logged in as admin %s.\n", user.username);
    }else if(status == 0){
        printf("Logged in as User %s.\n", user.username);
    }else{
        printf("Authentication Failed. Invalid credentials.\n");
    }
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
        printf("5) Exit\n");
    }else{
        drawline();
        printf("User Panel \n");
        drawline();
        printf("1) View all products\n");
        printf("2) Add product to cart\n");
        printf("3) View cart\n");
        printf("4) Remove items from cart\n");
        printf("5) Checkout\n");
        printf("6) Exit\n");
    }
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

void showCreateProductResult(int result){
    drawline();
    if(result < 0){
        printf("Pdt. creation failed.\n");
    }else{
        printf("Pdt. created successfully with id = %d\n A log has been generated in the logs folder.\n", result);
    }
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
    printf("Enter product quantity : \n");
    scanf("%d", &pdt.quantity);
    printf("Enter product price : \n");
    scanf("%lf", &pdt.price);
    return pdt;
}

void showUpdateProductResult(int result){
    if(result){
        printf("Product updated successfully.\n A log has been generated in the logs folder.\n");
    }else printf("Product updation failed.\n");
}

int showDeleteProduct(){
    int pdtid = -1;
    printf("Enter the id of the product to be deleted : \n");
    scanf("%d", &pdtid);
    return pdtid;
}

void showDeleteProductResult(int result){
    if(result){
        printf("Product deleted successfully.\n A log has been generated in the logs folder.\n");
    }else printf("Such a product was not found.\n");
}

struct Product showAddToCart(){
    struct Product temp;
    printf("Enter product id to be added to cart : \n");
    scanf("%d", &temp.id);
    printf("Enter product quantity to be added to cart : \n");
    scanf("%d", &temp.quantity);
    return temp;
} 

struct Product showSaveUserResult(int status){
    drawline();
    if(status){
        printf("User cart saved successfully!\n");
    }else printf("Error while saving user cart.\n");
    drawline();
}

void showAddToCartResult(int status){
    if(status){
        printf("Successfully added products to cart!\n");
    }else printf("Failed to add products to cart.\n");
}

struct Product showRemoveItemsFromCart(){
    struct Product temp;
    printf("Enter product id of the product : \n");
    scanf("%d", &temp.id);
    printf("Enter quantity to be removed : \n");
    scanf("%d", &temp.quantity);
    return temp;
}

void showRemoveItemsFromCartResult(int status){
    if(status){
        printf("Successfully removed products from cart!\n");
    }else printf("Failed to remove products from cart.\n");
}

double showCartItems(int n, struct Product cart[10]){   
    drawline(); 
    if(n == 0){
        printf("No items in Cart !");
        drawline();
        return 0;
    }
    double total = 0;
    for(int i=0;i<n;i++){
        printf("%d) %s : %d in cart - %0.2lf inr\n", cart[i].id, cart[i].name, cart[i].quantity, cart[i].price);
        total += cart[i].price*cart[i].quantity;
    }
    drawline();
    printf("TOTAL : %0.2lf inr\n", total);
    return total;
}

int showCheckoutSuccess(int n, struct Product cart[10]){
    double total = showCartItems(n, cart);
    printf("Please confirm payment by entering the displayed total again (just the integer part is fine)\n");
    double enteredtotal;
    scanf("%lf", &enteredtotal);
    if((int) enteredtotal == (int) total){
        return 1;
    }
    return 0;
}

void showCheckoutFailure(int n, struct Product cart[10]){
    printf("Some products in your cart are not available . They are listed below :\n");
    for(int i=0;i<n;i++){
        printf("%d) %s : %d in cart - %0.2lf inr\n", cart[i].id, cart[i].name, cart[i].quantity, cart[i].price);
    }
    drawline();
    printf("Please reduce quantity or try again later.\n");
    drawline();
}

void showPaymentResult(int status){
    if(status){
        printf("Products were successfully purchased :)\n A receipt has been generated in the logs file.\n");
    }else{
        printf("Could not complete payment :(.\n");
    }
}

#endif