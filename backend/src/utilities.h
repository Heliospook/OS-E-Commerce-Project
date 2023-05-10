#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include "headers.h"

#ifndef UTILITIES
#define UTILITIES

#define USERFILE "backend/database/users.dat"
#define PRODFILE "backend/database/products.dat"
// #define CARTFILE "backend/database/cart.dat"

union semun{
    int val;
    struct semid_ds *buff;
    unsigned short *array;
};

struct User login(char *username, char *password){
    struct User users[10];
    int numusers;
    FILE* userfile = (FILE *) fopen(USERFILE, "rb");
    fread(&numusers, sizeof(numusers), 1, userfile);
    fread(users, sizeof(struct User), numusers, userfile);
    for(int i=0;i<numusers;i++){
        if(strcmp(users[i].username, username) == 0){
            if(strcmp(users[i].password, password) == 0){
                return users[i];
            }
        }
    }
    struct User temp;
    temp.isAdmin = -1;
    return temp;
}

void createuser(){
    struct User newuser;
    printf("Enter name of user : \n");
    scanf("%s", newuser.username);
    printf("Create password : \n");
    scanf("%s", newuser.password);
    printf("Is the user an admin ? (0/1)\n");
    scanf("%d", &newuser.isAdmin);

    struct User users[10];
    int numusers = 0;
    FILE* userfile = (FILE *) fopen(USERFILE, "rb");
    fread(&numusers, sizeof(int), 1, userfile);
    fread(users, sizeof(struct User), numusers, userfile);
    users[numusers++] = newuser;
    fclose(userfile);
    
    userfile = (FILE *) fopen(USERFILE, "wb");
    fwrite(&numusers, sizeof(numusers), 1, userfile);
    fwrite(&users, sizeof(struct User), numusers, userfile);
    fclose(userfile);
}

int initsemaphores(int semid){
    int n = 0; //number of products
    FILE* productfile = (FILE *) fopen(PRODFILE, "rb+");
    fread(&n, sizeof(n), 1, productfile);
    struct Product pdts[256];
    fread(pdts, sizeof(struct Product), n, productfile);
    fclose(productfile);
    union semun args;
    unsigned short arr[256];
    for(int i=0;i<255;i++) arr[i] = 0;
    for(int i=0;i<n;i++) arr[i] = pdts[i].quantity;
    args.array = arr;

    int ret = semctl(semid, 0, SETALL, args);
    if(ret == -1){
        printf("Error initializing semaphores\n");
        return -1;
    }
    return n;
}

int createProduct(struct Product* pdt){
    int n = 0, newid = 0;
    FILE* productfile = (FILE *) fopen(PRODFILE, "rb+");
    fread(&n, sizeof(int), 1, productfile);
    fread(&newid, sizeof(int), 1, productfile);
    if(n >= 256) return PDT_CREATION_FAILED;
    
    pdt->id = newid;
    n++;
    fseek(productfile, 0, SEEK_SET);
    fwrite(&n, sizeof(int), 1, productfile);
    fseek(productfile, 2*sizeof(int) + newid*sizeof(struct Product), SEEK_SET);
    fwrite(pdt, sizeof(struct Product), 1, productfile);
    
    struct Product temp;
    newid++;
    while(fread(&temp, sizeof(struct Product), 1, productfile)){
        if(temp.id == -1) break;
        newid++;
    }
    fseek(productfile, sizeof(int), SEEK_SET);
    fwrite(&newid, sizeof(int), 1, productfile);
    fclose(productfile);

    return newid;
}

int readProducts(struct Product **products){
    FILE* productfile = (FILE *) fopen(PRODFILE, "rb");
    int n = 0;
    fread(&n, sizeof(int), 1, productfile);
    fseek(productfile, 2*sizeof(int), SEEK_SET);
    struct Product pdts[256], updPdts[256];
    struct Product temp; temp.id = -1;
    for(int i=0;i<256;i++) pdts[i] = temp;
    fread(pdts, sizeof(struct Product), 256, productfile);
    int curr = 0;
    for(int i=0;i<256;i++){
        if(pdts[i].id < 0) continue;
        updPdts[curr++] = pdts[i];
    }
    fclose(productfile);
    *products = updPdts;
    return n;
}

int updateProduct(struct Product pdt){
    FILE* productfile = (FILE *) fopen(PRODFILE, "rb+");

    int address = 2*sizeof(int) + pdt.id * sizeof(struct Product);
    fseek(productfile, address, SEEK_SET);
    struct Product temp;
    temp.id = -1;
    int rd = fread(&temp, sizeof(struct Product), 1, productfile);
    if(rd == 0 || temp.id < 0){
        fclose(productfile);
        return 0;
    }
    
    fseek(productfile, address, SEEK_SET);
    int wrt = fwrite(&pdt, sizeof(struct Product), 1, productfile);
    fclose(productfile);
    if(wrt == 0) return 0;
    return 1;
}

int deleteProduct(int pdtid){
    FILE* productfile = (FILE *) fopen(PRODFILE, "rb+");
    int n = 0;
    fread(&n, sizeof(int), 1, productfile);
    int newid = 0;
    fread(&newid, sizeof(int), 1, productfile);
    int address = 2*sizeof(int) + pdtid*sizeof(struct Product);
    fseek(productfile, 0, SEEK_END);
    int offset = ftell(productfile);
    if(offset <= address){
        return 0;
    }
    struct Product temp;
    fseek(productfile, address, SEEK_SET);
    fread(&temp, sizeof(struct Product), 1, productfile);
    if(temp.id != pdtid){
        return 0;
    }
    if(temp.id < newid){
        newid = temp.id;
        fseek(productfile, sizeof(int), SEEK_SET);
        fwrite(&newid, sizeof(int), 1, productfile);
    }
    n--;
    fseek(productfile, 0, SEEK_SET);
    fwrite(&n, sizeof(int), 1, productfile);
    temp.id = -1;
    fseek(productfile, address, SEEK_SET);
    fwrite(&temp, sizeof(struct Product), 1, productfile);
    fclose(productfile);

    return 1;
}


void writeone(){
    int n = 0;
    FILE* productfile = (FILE *) fopen(PRODFILE, "wb+");
    fwrite(&n, sizeof(n), 1, productfile);
}

void printProduct(struct Product pdt){
    printf("%d %s %d\n", pdt.id, pdt.name, pdt.quantity);
}

#endif
