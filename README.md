
# OS E-Commerce Project
### Subhajeet Lahiri (IMT2021022)

An E-Commerce Platform for phones with both the front-end and the back-end written in C. The interface on the front-end is text-based. In the backend, a file-based datastore is used, along with flocks and semaphores for concurrency control.



## 🔗 Github
[![Github](https://img.shields.io/badge/Heliospook-000?style=for-the-badge&logo=github&logoColor=white)](https://github.com/Heliospook/OS-E-Commerce-Project.git)

If the badge doesn't work, use : 
https://github.com/Heliospook/OS-E-Commerce-Project.git


## Demo
### Getting the executables ready
The project has been supplied with a makefile.
Run the makefile in the root directory as follows to create the executables :
```bash
    make clean
    make main
```
### Running the server
Get the server running using :
```bash
    ./server
```
### Connecting with clients
To connect a client to the running server, open a new terminal and run the client executable.
```bash
    ./client
```

## Features

### Symmetric admin/ user interface  
   Both of the above interfaces can be accessed by running the `client` executable. The server only interacts with the user through the client.
Whether a user is an admin or not is decided by the isAdmin property of the user object.  
While logging in, the user is expected to enter the username and password. Some pre-created logins are :   
    - username : me, password : pass (admin)  
    - username : you, password : pass (customer)  
    - username : another, password : pass (customer)

These can be used to test the application.

### Concurrency control

Several different types of concurrency control mechanisms have been used throughout the application

#### Login  
The list of all users has been stored in the users.dat file. To prevent issues arising out of simultaneous access by the same user, **the corresponding record for the user is placed under a write lock(flock) while the user is logged in.**

#### Checkout
Semaphores have been used to address concurrency issues arising out of multiple people trying to purchase the same product. In my implementation,  
**Semaphore value = No. of products left in stock**  
Each product has a corresponding semaphore attached to it. A user is allowed to **lock** a product by subtracting the reqd. quantity, if the available **unlocked** quantity is adequate, as indicated by the semaphore value.

#### Admin updation
When the admin updates a product, it is placed under a total lock by decreasing the semaphore value to 0. When the admin is done, the value is updated to the new quantity.

### File I/O

To ensure max. concurrency, changes are synced to files as soon as possible, perhaps at the cost of efficiency. However, this is necessary to have consistent data across multiple sessions by various customers and admins.


### Client-Server Model 

The concurrent server is set-up by spawning **child processes** when a new client connects. The client-server model is respected by ensuring that none of the computation/data-handling is done in the client. The client uses the ux.h header file to render the interface and relays everything back to the server for processing. The server calls the functions in the utilities.h file to perform the required operations. To ensure that the client and the server communicate on the same terms, the headers.h file is used by both of them.


## Execution

Here are some screenshots of the execution of the client.

### Admin

Logging in as admin 

![a1](https://firebasestorage.googleapis.com/v0/b/shopapp-8e2eb.appspot.com/o/a1.png?alt=media&token=be0539bf-5920-4cc2-9cd3-37c1bd557d05)

View all products 

![a2](https://firebasestorage.googleapis.com/v0/b/shopapp-8e2eb.appspot.com/o/a2.png?alt=media&token=328e1be8-2470-43f6-8203-7aeca018bc5e)

Creating a product 

![a3](https://firebasestorage.googleapis.com/v0/b/shopapp-8e2eb.appspot.com/o/a3.png?alt=media&token=47eb0fdc-f696-4c93-b330-ce2dc5cb46b0)

Deleting a product

![a4](https://firebasestorage.googleapis.com/v0/b/shopapp-8e2eb.appspot.com/o/a4.png?alt=media&token=84071b65-02ab-467a-8e8e-a9310a42c8cb)

Updating a product 

![a5](https://firebasestorage.googleapis.com/v0/b/shopapp-8e2eb.appspot.com/o/a5.png?alt=media&token=6e70465e-f99d-49e2-94f9-99ce58881e7e)

![a6](https://firebasestorage.googleapis.com/v0/b/shopapp-8e2eb.appspot.com/o/a6.png?alt=media&token=cc796cd9-9c96-4e14-a10d-6498fcf8306e)

### Customer

If a customer is already logged in, and we try from another terminal using the same credentials, then we get an error.

![c1](https://firebasestorage.googleapis.com/v0/b/shopapp-8e2eb.appspot.com/o/c1.png?alt=media&token=e4f64440-a7c0-400b-80fd-f2a678172bc8)

Adding products to cart

![c2](https://firebasestorage.googleapis.com/v0/b/shopapp-8e2eb.appspot.com/o/c2.png?alt=media&token=485b31e1-16d4-4337-9a7c-860bff60c481)

Viewing the cart

![c3](https://firebasestorage.googleapis.com/v0/b/shopapp-8e2eb.appspot.com/o/c3.png?alt=media&token=58c492f7-5599-452d-920e-f1bd45fdbcbc)

Removing products from cart

![c4](https://firebasestorage.googleapis.com/v0/b/shopapp-8e2eb.appspot.com/o/c4.png?alt=media&token=2012963c-2813-4ae8-bb69-407de895d0a6)

Suppose two users have such quantities of the same product in their carts that both the orders can't be satisfied :

![c5a](https://firebasestorage.googleapis.com/v0/b/shopapp-8e2eb.appspot.com/o/c5a.png?alt=media&token=2470af59-e683-4226-b4b3-9ff31e7f5321)

![c5b](https://firebasestorage.googleapis.com/v0/b/shopapp-8e2eb.appspot.com/o/c5b.png?alt=media&token=0c53a2d5-3466-4742-b385-6e4f78a97b63)

Then the user who checks out first will be allowed to do so : 

![c6a](https://firebasestorage.googleapis.com/v0/b/shopapp-8e2eb.appspot.com/o/c6a.png?alt=media&token=16d8b0c2-afaf-47d4-bd30-459310474b17)

While the other who couldn't would have to reduce the quantity.

![c6b](https://firebasestorage.googleapis.com/v0/b/shopapp-8e2eb.appspot.com/o/c6b.png?alt=media&token=39d3b2f4-e398-4f78-a0a9-c8f6f72c413f)

Successful payment

![c7](https://firebasestorage.googleapis.com/v0/b/shopapp-8e2eb.appspot.com/o/c7.png?alt=media&token=d3bfafc1-1b33-4187-9e3a-088b2f75bc56)


