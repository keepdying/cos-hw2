#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "shopping.h"

struct paymentArgs {
    struct Customer *customer;
    struct Product *products;
    int num_products;
};

pthread_mutex_t lock;

void *processPayment(void *arg){
    struct Customer *customer = ((struct paymentArgs *)arg)->customer;
    struct Product *products = ((struct paymentArgs *)arg)->products;
    int num_products = ((struct paymentArgs *)arg)->num_products;

    for (int i = 0; i < customer->num_ordered_Items; i++) {
        int product_ID = customer->ordered_Items[i].product_ID - 1;
        int quantity = customer->ordered_Items[i].product_Quantity;
        
        pthread_mutex_lock(&lock);
        if (products[product_ID].product_Quantity >= quantity) {
            if(customer->customer_Balance >= (products[product_ID].product_Price * quantity)){
                products[product_ID].product_Quantity -= quantity;
                pthread_mutex_unlock(&lock);
                customer->purchased_Items[i] = customer->ordered_Items[i];
                customer->num_purchased_Items++;
                customer->customer_Balance -= (products[product_ID].product_Price * quantity);
                printf("Customer %d has purchased %d of product %d\n", customer->customer_ID, quantity, (product_ID + 1));
            } else {
                pthread_mutex_unlock(&lock);
                printf("Customer %d does not have enough money to purchase %d of product %d\n", customer->customer_ID, quantity, (product_ID + 1));
            }
        } else {
            pthread_mutex_unlock(&lock);
            printf("Customer %d couldn't purchase %d of product %d since not enought quantity.\n", customer->customer_ID, quantity, (product_ID + 1));
        }
    }
}


int main(int argc, char *argv[]){

    srand(time(NULL));
    int num_products = NUM_PRODUCTS;
    struct Product *products = malloc(sizeof(struct Product) * num_products);

    for (int i = 0; i < num_products; i++) {
        products[i] = createRandomProduct((i + 1));
    }

    printf("Initial Products: \n");
    printProducts(products, num_products);

    int num_customers = NUM_CUSTOMERS;
    struct Customer *customers = malloc(sizeof(struct Customer) * num_customers);
    for (int i = 0; i < num_customers; i++) {
        customers[i] = createRandomCustomer((i + 1), products, num_products);
    }

    printf("Initial Customers: \n");

    printCustomers(customers, num_customers);
    
    pthread_t tids[num_customers];

    for (int i = 0; i < num_customers; i++) {
        struct paymentArgs *args = malloc(sizeof(struct paymentArgs));
        args->customer = &customers[i];
        args->products = products;
        args->num_products = num_products;
        pthread_create(&tids[i], NULL, processPayment, args);
    }

    for (int i = 0; i < num_customers; i++) {
        pthread_join(tids[i], NULL);
    }
    
    pthread_mutex_destroy(&lock);
    return 0;
}