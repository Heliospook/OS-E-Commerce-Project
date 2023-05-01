#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include "headers.h"

#ifndef UTILITIES
#define UTILITIES

union semun{
    int val;
    struct semid_ds *buff;
    unsigned short *array;
};

int login(char *username, char *password){
    struct User users[10];
    int numusers;
    FILE* userfile = (FILE *) fopen("database/users.dat", "rb");
    fread(&numusers, sizeof(numusers), 1, userfile);
    fread(users, sizeof(struct User), numusers, userfile);
    for(int i=0;i<numusers;i++){
        if(strcmp(users[i].username, username) == 0){
            if(strcmp(users[i].password, password) == 0){
                return users[i].isAdmin;
            }
        }
    }
    return -1;
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
    FILE* userfile = (FILE *) fopen("database/users.dat", "rb");
    fread(&numusers, sizeof(numusers), 1, userfile);
    fread(users, sizeof(struct User), numusers, userfile);
    users[numusers++] = newuser;
    fclose(userfile);
    
    userfile = (FILE *) fopen("database/users.dat", "wb");
    fwrite(&numusers, sizeof(numusers), 1, userfile);
    fwrite(&users, sizeof(struct User), numusers, userfile);
    fclose(userfile);
}

int initsemaphores(int semid){
    int n = 0; //number of products
    FILE* productfile = (FILE *) fopen("database/products.dat", "rb+");
    fread(&n, sizeof(n), 1, productfile);
    struct Product pdts[255];
    fread(pdts, sizeof(struct Product), n, productfile);
    fclose(productfile);

    union semun args;
    unsigned short arr[256];
    for(int i=0;i<255;i++) arr[i] = 0;
    for(int i=0;i<n;i++) arr[i] = pdts[i].quantity;
    args.array = arr;

    int ret = semctl(semid, 0, SETALL, args);
    perror("semctl");
    if(ret == -1){
        printf("Error initializing semaphores\n");
        return -1;
    }
    return n;
}

int createProduct(struct Product* pdt){
    int result = PDT_CREATION_FAILED;
    FILE* productfile = (FILE *) fopen("database/products.dat", "rb+");
    fread(&result, sizeof(int), 1, productfile);
    if(result < 256){
        pdt->id = result;
        result++;
        fseek(productfile, 0, SEEK_SET);
        fwrite(&result, sizeof(int), 1, productfile);
        fseek(productfile, 0, SEEK_END);
        fwrite(pdt, sizeof(struct Product), 1, productfile);
        fclose(productfile);
    }
    return result;
}

int readProducts(struct Product **products){
    FILE* productfile = (FILE *) fopen("database/products.dat", "rb");
    int n = 0;
    fread(&n, sizeof(n), 1, productfile);
    struct Product pdts[256];
    fread(pdts, sizeof(struct Product), n, productfile);
    fclose(productfile);
    *products = pdts;
    return n;
}

void writeone(){
    int n = 0;
    FILE* productfile = (FILE *) fopen("database/products.dat", "wb+");
    fwrite(&n, sizeof(n), 1, productfile);
}

#endif