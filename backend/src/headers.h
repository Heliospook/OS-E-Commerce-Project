#ifndef HEADERS
#define HEADERS

#define PDT_CREATION_FAILED -10

struct User{
    char username[100];
    char password[100];
    int isAdmin;
};

struct Product{
    int id;
    char name[100];
    int quantity;
    double price;
};

#endif