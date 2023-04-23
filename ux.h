#include <stdio.h>

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
        printf("3) Delete a product\n");
        printf("4) Modify a product\n");
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


#endif