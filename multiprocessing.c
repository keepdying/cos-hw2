#include <stdio.h> // Standard input/output
#include <stdlib.h> // Standard library
#include <string.h> // String operations
#include <unistd.h> // Standard symbolic constants and types
#include <sys/mman.h> // Memory management declarations
#include <pthread.h> // POSIX threads

#include "shopping.h" // Shopping header file

struct productsShMem { // Shared memory for products
    struct Product products[NUM_PRODUCTS]; // Products
    int num_products; // Number of products 
    pthread_mutex_t lock; // Mutex lock
};

struct paymentArgs {
    struct Customer *customer;
    struct Product *products;
    int num_products;
    pthread_mutex_t *lock;
};

void* create_shared_memory(size_t size) {
  // Our memory buffer will be readable and writable:
  int protection = PROT_READ | PROT_WRITE;

  // The buffer will be shared (meaning other processes can access it), but
  // anonymous (meaning third-party processes cannot obtain an address for it),
  // so only this process and its children will be able to use it:
  int visibility = MAP_SHARED | MAP_ANONYMOUS;

  // The remaining parameters to `mmap()` are not important for this use case,
  // but the manpage for `mmap` explains their purpose.
  return mmap(NULL, size, protection, visibility, -1, 0);
}

void processPayment(void *arg){
    struct Customer *customer = ((struct paymentArgs *)arg)->customer;
    struct Product *products = ((struct paymentArgs *)arg)->products;
    int num_products = ((struct paymentArgs *)arg)->num_products;
    pthread_mutex_t *lock = ((struct paymentArgs *)arg)->lock;

    #if SCENARIO == 2
    pthread_mutex_lock(lock);
    #endif

    for (int i = 0; i < customer->num_ordered_Items; i++) {
        int product_ID = customer->ordered_Items[i].product_ID - 1;
        int quantity = customer->ordered_Items[i].product_Quantity;

        #if SCENARIO == 1
        pthread_mutex_lock(lock);
        #endif

        if (products[product_ID].product_Quantity >= quantity) {
            if(customer->customer_Balance >= (products[product_ID].product_Price * quantity)){
                products[product_ID].product_Quantity -= quantity;
                customer->purchased_Items[i] = customer->ordered_Items[i];
                customer->num_purchased_Items++;
                customer->customer_Balance -= (products[product_ID].product_Price * quantity);
                printf("Customer%d(%d,%d) success! Paid %f$ for each.\n", customer->customer_ID, (product_ID + 1), quantity, products[product_ID].product_Price);
            } else {
                printf("Customer%d(%d,%d) fail! Insufficent funds.\n", customer->customer_ID, (product_ID + 1), quantity);
            }
        } else {
            if (products[product_ID].product_Quantity > 0){
                printf("Customer%d(%d,%d) fail! Only left %d in stock.\n", customer->customer_ID, (product_ID + 1), quantity, products[product_ID].product_Quantity);
            } else {
                printf("Customer%d(%d,%d) fail! Out of stock.\n", customer->customer_ID, (product_ID + 1), quantity);
            }
        }

        #if SCENARIO == 1
        pthread_mutex_unlock(lock);
        #endif

    }
    #if SCENARIO == 2
    pthread_mutex_unlock(lock);
    #endif
}

void processChildPayments(struct Customer *customer, struct productsShMem *productsShMem){
    struct paymentArgs *args = malloc(sizeof(struct paymentArgs)); // Allocate memory for payment arguments
    args->customer = customer; // Set customer
    args->products = &productsShMem->products[0]; // Set products
    args->num_products = NUM_PRODUCTS; // Set number of products
    args->lock = &productsShMem->lock; // Set mutex lock
    processPayment(args); // Process payment
}

int main(int argc, char** argv){

    srand(time(NULL)); // Seed random number generator
    struct Product *products = malloc(sizeof(struct Product) * NUM_PRODUCTS); // Allocate memory for products

    for (int i = 0; i < NUM_PRODUCTS; i++) {
        products[i] = createRandomProduct((i + 1)); // Create random products
    }

    struct Customer *customers = malloc(sizeof(struct Customer) * NUM_CUSTOMERS); // Allocate memory for customers
    for (int i = 0; i < NUM_CUSTOMERS; i++) {
        customers[i] = createRandomCustomer((i + 1), products, NUM_PRODUCTS); // Create random customers
    }

    printf("Initial Customers: \n");
    printCustomers(customers, NUM_CUSTOMERS); // Print initial customers

    pid_t pid[NUM_CUSTOMERS]; // Array of process IDs
    struct productsShMem *productsShMem = create_shared_memory(sizeof(struct productsShMem)); // Create shared memory for products
    memcpy(&productsShMem->products, products, sizeof(struct Product) * NUM_PRODUCTS); // Copy products to shared memory
    pthread_mutexattr_t attr; // Mutex lock attributes
    pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED); // Set mutex lock to shared
    pthread_mutex_init(&productsShMem->lock, &attr); // Initialize mutex lock
    
    free(products); // Free memory for products
    
    printf("Initial Products: \n");
    printProducts(&productsShMem->products[0], NUM_PRODUCTS); // Print initial products
    
    for (int i = 0; i < NUM_CUSTOMERS; i++) {
        pid[i] = fork(); // Fork process
        if (pid[i] == 0){
            processChildPayments(&customers[i], productsShMem); // Process child payments
            exit(0); // Exit child process
        }

    }
    
    for (int i = 0; i < NUM_CUSTOMERS; i++) {
        int status;
        waitpid(pid[i], &status, 0);
        if (WIFEXITED(status)) {
            printf("Child %d exited with status %d\n", i, WEXITSTATUS(status));
        }
    }

    printf("Final Products: \n");
    printProducts(&productsShMem->products[0], NUM_PRODUCTS); // Print final products

    pthread_mutex_destroy(&productsShMem->lock); // Destroy mutex lock
    munmap(productsShMem, sizeof(struct productsShMem)); // Unmap shared memory
    
    free(customers); // Free memory for customers

    return 0;
}