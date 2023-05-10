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

union semun{
    int val;
    struct semid_ds *buff;
    unsigned short *array;
};

struct User login(char *username, char *password){
    int numusers;
    int fd = open(USERFILE, O_RDWR | O_CREAT, 0777);
    read(fd, &numusers, sizeof(int));

    struct User curruser;
    for(int i=0;i<numusers;i++){
        read(fd, &curruser, sizeof(struct User));
        if(strcmp(curruser.username, username) == 0){
            if(strcmp(curruser.password, password) == 0){
                struct flock lck;
                lck.l_type = F_WRLCK;
                lck.l_whence = SEEK_SET;
                lck.l_start = sizeof(int) + i*sizeof(struct User);
                lck.l_len = sizeof(struct User);
                
                int ret = fcntl(fd, F_SETLK, &lck);
                if(ret < 0) break;
                return curruser;
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
    newuser.nCart = 0;

    struct User users[10];
    int numusers = 0;
    FILE* userfile = (FILE *) fopen(USERFILE, "rb+");
    fread(&numusers, sizeof(int), 1, userfile);
    fread(users, sizeof(struct User), numusers, userfile);
    if(numusers>=10){
        printf("Can't create user, user list full.\n");
        return;
    }
    users[numusers++] = newuser;
    fseek(userfile, 0, SEEK_SET);
    fwrite(&numusers, sizeof(numusers), 1, userfile);
    fwrite(&users, sizeof(struct User), numusers, userfile);
    fclose(userfile);
}

int saveuser(struct User user){
    int numusers;
    int fd = open(USERFILE, O_RDWR | O_CREAT, 0777);
    read(fd, &numusers, sizeof(int));
    int index = -1;
    struct User curruser;
    for(int i=0;i<numusers;i++){
        read(fd, &curruser, sizeof(struct User));
        if(!strcmp(curruser.username, user.username) && !strcmp(curruser.password, user.password)){
            index = i;
        }
    }
    if(index == -1) return 0;
    int offset = sizeof(int) + index*sizeof(struct User);
    lseek(fd, offset, SEEK_SET);
    write(fd, &user, sizeof(struct User));
    
    struct flock lck;
    lck.l_type = F_UNLCK;
    lck.l_whence = SEEK_SET;
    lck.l_start = offset;
    lck.l_len = sizeof(struct User);
    int ret = fcntl(fd, F_SETLK, &lck);
    if(ret < 0) return 0;
    return 1;
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

int initsemaphores(int semid){
    struct Product *pdts = (struct Product *) malloc(256*sizeof(struct Product));
    int n = readProducts(&pdts);

    union semun args;
    args.array = (unsigned short *) malloc(256 * sizeof(unsigned short));

    for(int i=0;i<256;i++) args.array[i] = 0;
    for(int i=0;i<n;i++){
        if(pdts[i].quantity > 0){
            args.array[i] = pdts[i].quantity;
        }
    }

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
    
    int ret = newid;
    struct Product temp;
    newid++;
    while(fread(&temp, sizeof(struct Product), 1, productfile)){
        if(temp.id == -1) break;
        newid++;
    }
    fseek(productfile, sizeof(int), SEEK_SET);
    fwrite(&newid, sizeof(int), 1, productfile);
    fclose(productfile);

    return ret;
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

int fetchProductById(int pdtid, struct Product *pdt){
    struct Product *pdts = (struct Product *) malloc(256*sizeof(struct Product));
    int n = readProducts(&pdts);

    for(int i=0;i<n;i++){
        if(pdts[i].id == pdtid){
            *pdt = pdts[i];
            return 1;
        }
    }
    return 0;
}   

int addToCart(struct Product *pdt, struct User *user){
    int found = -1;
    for(int i=0;i<user->nCart;i++){
        if(user->cart[i].id == pdt->id){
            found = i;
            pdt->quantity += user->cart[i].quantity;
        }
    }

    struct Product storepdt;
    int status = fetchProductById(pdt->id, &storepdt);
    if(!status) return 0;
    strcpy(pdt->name, storepdt.name);
    pdt->price = storepdt.price;
    if(storepdt.quantity < pdt->quantity) return 0;

    if(found == -1){
        if(user->nCart >= 10) return 0;
        user->cart[(user->nCart)++] = *pdt;
    }else{
        user->cart[found] = *pdt;
    }
    return 1;
}
int removeFromCart(struct Product *pdt, struct User *user){
    int index = -1;
    for(int i=0;i<user->nCart;i++){
        if(user->cart[i].id == pdt->id){
            if(user->cart[i].quantity >= pdt->quantity){
                user->cart[i].quantity -= pdt->quantity;
                index = i;
            }else return 0;
        }
    }
    if(index == -1) return 0;
    if(user->cart[index].quantity == 0){
        for(int i=index;i<user->nCart - 1;i++){
            user->cart[i] = user->cart[i+1];
        }
        user->nCart--;
    }
    return 1;
}

int lockCart(struct Product cart[10], int n, struct Product failedcart[10], int* nfails, int semid){
    int curr = 0;
    for(int i=0;i<n;i++){
        int state = semctl(semid, cart[i].id, GETVAL, NULL);
        printf("cart item %d %d\n", cart[i].id, state);
        if(cart[i].quantity > state){
            failedcart[curr++] = cart[i];
        }
    }
    *nfails = curr;
    if(curr) return 0;
    
    for(int i=0;i<n;i++){
        struct sembuf buf = {
            cart[i].id,
            -cart[i].quantity,
            IPC_NOWAIT
        };
        int ret = semop(semid, &buf, 1);
        int currval = semctl(semid, cart[i].id, GETVAL, NULL);
        printf("%d\n", currval);
    }
    return 1;
}


int unlockCart(struct Product cart[10], int n, int semid){
    for(int i=0;i<n;i++){
        struct sembuf buf = {
            cart[i].id,
            cart[i].quantity,
            IPC_NOWAIT | SEM_UNDO
        };
        semop(semid, &buf, 1);
        perror("unlock");
    }
    return 1;
}

int purchaseAll(struct Product cart[10], int n){
    FILE* productfile = (FILE *) fopen(PRODFILE, "rb+");
    for(int i=0;i<n;i++){
        int offset = 2*sizeof(int) + cart[i].id*sizeof(struct Product);
        fseek(productfile, offset, SEEK_SET);
        struct Product temp;
        fread(&temp, sizeof(struct Product), 1, productfile);
        temp.quantity -= cart[i].quantity;
        fseek(productfile, offset, SEEK_SET);
        fwrite(&temp, sizeof(struct Product), 1, productfile);
    }
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

void getsemvals(int semid){
    for(int i=0;i<10;i++){
        int val = semctl(semid, i, GETVAL, NULL);
        printf("%d ", val);
    }
    printf("\n");
}

#endif
