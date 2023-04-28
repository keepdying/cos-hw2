#include <stdio.h>
#include <stdlib.h>
#include "shopping.h"




int main(char** argv, int argc){

    int num_products = NUM_PRODUCTS;
    struct Product *products = malloc(sizeof(struct Product) * num_products);

    for (int i = 0; i < num_products; i++) {
        products[i] = createRandomProduct(i);
    }

    printf("Initial Products: \n");
    printProducts(products, num_products);

    int num_customers = NUM_CUSTOMERS;
    struct Customer *customers = malloc(sizeof(struct Customer) * num_customers);
    for (int i = 0; i < num_customers; i++) {
        customers[i] = createRandomCustomer(i, products, num_products);
    }

    printf("Initial Customers: \n");

    printCustomers(customers, num_customers);
    
    return 0;
}