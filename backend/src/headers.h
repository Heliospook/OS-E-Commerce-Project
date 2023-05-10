#ifndef HEADERS
#define HEADERS

#define PDT_CREATION_FAILED -10

struct Product{
    int id;
    char name[100];
    int quantity;
    double price;
};

struct User{
    char username[100];
    char password[100];
    int isAdmin;
    struct Product cart[10];
};


#endif