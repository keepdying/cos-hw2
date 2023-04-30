#ifndef SHOPPING_H
#define SHOPPING_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define NUM_PRODUCTS 5
#define NUM_CUSTOMERS 3
#define MAX_ORDERED_ITEMS 10
#define MAX_PURCHASED_ITEMS 10

struct Product {
    int product_ID;
    float product_Price;
    int product_Quantity;
    /* add more members if needed */
};

struct Product createProduct(int id, int quantity, float price);
void printProducts(struct Product *products, int num_products);
struct Product createRandomProduct(int id);

struct Customer {
    int customer_ID;
    float customer_Balance;
    struct Product *ordered_Items;
    struct Product *purchased_Items;
    int num_ordered_Items;
    int num_purchased_Items;
    /* add more members if needed */
};

void printCustomers(struct Customer *customers, int num_customers);
struct Customer createCustomer(int id, float balance);
struct Customer createRandomCustomer(int id, struct Product *products, int num_products);

#endif /* SHOPPING_H */
