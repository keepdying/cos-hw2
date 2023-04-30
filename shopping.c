#include "shopping.h"

struct Product createProduct(int id, int quantity, float price) {
    struct Product p;
    p.product_ID = id;
    p.product_Quantity = quantity;
    p.product_Price = price;
    return p;
}

void printProducts(struct Product *products, int num_products) {
    // Print the header for the product list
    // printf("Initial products:\n");
    printf("Product ID Quantity Price\n");
    // Loop through the product array and print out each product's information
    for (int i = 0; i < num_products; i++) {
        printf("%d %d %.2f\n", products[i].product_ID, products[i].product_Quantity, products[i].product_Price);
    }
}

struct Product createRandomProduct(int id) {
    // Generate random values for product_Price and product_Quantity
    int quantity = rand() % 10 + 1; // generate a random number between 1 and 10
    float price = ((float)rand() / RAND_MAX) * 199.0 + 1.0; // generate a random float between 1.0 and 200.0
    // Create and return a new Product struct with the random values and provided id
    struct Product p = createProduct(id, quantity, price);
    return p;
}

void printCustomers(struct Customer *customers, int num_customers){
    for (int i = 0; i < num_customers; i++) {
        printf("Customer ID Balance\n");
        printf("%d %.2f\n", customers[i].customer_ID, customers[i].customer_Balance);
        printf("Ordered Items:\n");
        printProducts(customers[i].ordered_Items, customers[i].num_ordered_Items);
        printf("--------------------\n");
    }
}

struct Customer createCustomer(int id, float balance) {
    struct Customer c;
    c.customer_ID = id;
    c.customer_Balance = balance;

    // Initialize the ordered_Items and purchased_Items arrays to empty
    c.num_ordered_Items = 0;
    c.num_purchased_Items = 0;

    c.ordered_Items = malloc(sizeof(struct Product) * MAX_ORDERED_ITEMS);
    c.purchased_Items = malloc(sizeof(struct Product) * MAX_PURCHASED_ITEMS);

    for (int i = 0; i < MAX_ORDERED_ITEMS; i++) {
        c.ordered_Items[i] = createProduct(0, 0, 0.0);
    }

    for (int i=0; i < MAX_PURCHASED_ITEMS; i++) {
        c.purchased_Items[i] = createProduct(0, 0, 0.0);
    }

    return c;
}

struct Customer createRandomCustomer(int id, struct Product *products, int num_products) {
    // Generate a random value for customer_Balance
    float balance = ((float)rand() / RAND_MAX) * 2000.0; // generate a random float between 0.0 and 2000.0
    // Create and return a new Customer struct with the random value and provided id
    struct Customer c = createCustomer(id, balance);
    int num_ordered_Items = rand() % MAX_ORDERED_ITEMS; // generate a random number between 0 and MAX_ORDERED_ITEMS
    c.num_ordered_Items = num_ordered_Items;

    for (int i = 0; i < num_ordered_Items; i++) {
        int product_ID = rand() % num_products; // generate a random number between 0 and NUM_PRODUCTS
        c.ordered_Items[i] = products[product_ID];
    }

    return c;
}
