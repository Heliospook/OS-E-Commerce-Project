#include <stdio.h>
#include <string.h>
#include "headers.h"

#ifndef UTILITIES
#define UTILITIES

int login(char *username, char *password){
    struct User users[10];
    int numusers;
    FILE* userfile = (FILE *) fopen("users.dat", "rb");
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
    FILE* userfile = (FILE *) fopen("users.dat", "rb");
    fread(&numusers, sizeof(numusers), 1, userfile);
    fread(users, sizeof(struct User), numusers, userfile);
    users[numusers++] = newuser;
    fclose(userfile);
    
    userfile = (FILE *) fopen("users.dat", "wb");
    fwrite(&numusers, sizeof(numusers), 1, userfile);
    fwrite(&users, sizeof(struct User), numusers, userfile);
    fclose(userfile);
}


#endif