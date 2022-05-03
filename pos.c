/**
 * @file pos.c
 * @author Neil Jason Cañete (caneteneiljason@smccnasipit.edu.ph)
 * @date 2022-05-02
 * 
 * @copyright Copyright (c) Neil Jason Cañete @ SMCC 2022
 * @brief 
 * CREATE A POINT OF SALE (POS) SYSTEM.
 * The program should must these following major features:
 * > (Add, Display, Search, Update and Delete) Product details
 * > (Add, Display, Search, Update and Delete) Teller details
 * > Sale transaction
 * # PRODUCT DETAILS # should accept these following data:
 * > Product ID (should be unique)
 * > Product Name
 * > Product Description (this would describe the product)
 * > Product Category (example: dairy products, soft drinks, hygiene products etc.)
 * > Product Unit (piece or kilo)
 * > Product Unit Price
 * # TELLER DETAILS # should accept these following data:
 * > Teller ID (should be unique)
 * > Teller Name
 * # SALE TRANSACTION # should accept these following data:
 * > Sale ID (should be unique)
 * > Product Name
 * > Product Unit
 * > Product Price
 * > Quantity
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#ifdef _WIN32 // for Windows OS only
#include <conio.h>
void clrscr(void) {
    system("cls");
}
#elif __linux__ // for Linux OS only
#include <termios.h>
void clrscr(void)
{
    system("clear");
}
static struct termios old, new;
void initTermios(int echo)
{
    tcgetattr(0, &old); //grab old terminal i/o settings
    new = old; //make new settings same as old settings
    new.c_lflag &= ~ICANON; //disable buffered i/o
    new.c_lflag &= echo ? ECHO : ~ECHO; //set echo mode
    tcsetattr(0, TCSANOW, &new); //apply terminal io settings
}
void resetTermios(void)
{
    tcsetattr(0, TCSANOW, &old);
}
char getch_(int echo)
{
    char ch;
    initTermios(echo);
    ch = getchar();
    resetTermios();
    return ch;
}
char getch(void)
{
    return getch_(0);
}
char getche(void)
{
    return getch_(1);
}
#endif

// Define constants
#define MAX_NAME 250
#define MAX_ITEMS 2000

// Define Structures
typedef struct {
    int id;
    char name[MAX_NAME];
    char description[MAX_NAME];
    char category[MAX_NAME];
    char unit[MAX_NAME];
    float unit_price;
} Product; // Product Details
typedef struct {
    int id;
    char first_name[MAX_NAME];
    char middle_name[MAX_NAME];
    char last_name[MAX_NAME];
} Teller; // Teller Details
typedef struct {
    Product product;
    int quantity;
} ProductItem; // Item with quantity bought
typedef struct {
    int id;
    Teller teller; // teller details
    ProductItem items[MAX_ITEMS]; // products/items bought
    float payable_amount;
    float cash;
    float change;
} SaleTransaction; // Sale Transaction with teller details & Item bought

// Define Global Variables


// Define Function Prototypes
int CLI(void); // Command Line Interface
void prod_menu(void); // Product Details Menu
void teller_menu(void); // Teller Details Menu
void sales_menu(void); // Sale Transaction Menu
int prod_add(void); // Add new Product Details
void prod_display(void); // Display all Product Details
void prod_sud_menu(const char * request); // Product Search/Update/Delete Menu
void prod_search_id(int id, const char * request); // Product Search/Update/Delete Request by ID
void prod_search_name(const char * prod_name, const char * request); // Product Search/Update/Delete Request by Product Name
int teller_add(void); // Add new Teller Details
void teller_display(void); // Display all Teller Details
void teller_sud_menu(const char * request); // Teller Search/Update/Delete Menu
void teller_search_id(int id, const char * request); // Teller Search/Update/Delete Request by ID
void teller_search_name(const char * teller_name, const char * request); // Teller Search/Update/Delete Request by Product Name
void sale_new(void); // add new transaction
void sale_display(void); // Display Transactions
float compute_payable_amount(SaleTransaction * sale); // Compute Total Payable amount
float compute_change(SaleTransaction * sale); // Compute Total Payable amount
int getProductByID(Product * productBuffer, int id); // Get Product data to buffer by ID
// other function prototypes
int dscanc(int * d); // user single-input integer
int cscanc(char * c); // user single-input char
char * capitalize(const char * word); // Capitalize first letter of the word/string

int main(int argc, char * argv[]) {
    while (1) {
        if (CLI() == 4) // 4 = exit
            break;
    }
    printf("Thank you for using this Point of Sales System (POS). Goodbye!");
    getch(); // pause before exit
    return 0;
}

// Define Functions
/**
 * @brief Command Line User Interface Main Menu
 * 
 * @return int 4 - EXIT APPLICATION; else Loop back to Main menu
 */
int CLI(void) {
    clrscr();
    int choice = 0;
    printf("---------- Welcome To NJ Enterprise ----------\n");
    printf("Please select:\n\n");
    printf("[1] Product Details\n");
    printf("[2] Teller Details\n");
    printf("[3] Sale Transaction\n");
    printf("[4] Exit Program\n");
    printf("----------------------------------------------\n\n");
    do {
        printf("Enter Choice: ");
        dscanc(&choice);
        if (!(choice > 0 && choice < 5))
            printf("Invalid Choice!\n");
    } while (!(choice > 0 && choice < 5));
    switch (choice) {
        case 1:
            prod_menu();
            break;
        case 2:
            teller_menu();
            break;
        case 3:
            sales_menu();
            break;
    }
    return choice;
}
/**
 * @brief Product Details Menu
 * 
 */
void prod_menu(void) {
    clrscr();
    int choice;
    printf("---------- Product Details ----------\n\n");
    printf("[1] Add\n");
    printf("[2] Display\n");
    printf("[3] Search\n");
    printf("[4] Update\n");
    printf("[5] Delete\n");
    printf("[6] Go Back\n");
    printf("-------------------------------------\n\n");
    do {
        printf("Choice: ");
        dscanc(&choice);
        if (!(choice > 0 && choice < 7))
            printf("Invalid Choice!\n");
    } while (!(choice > 0 && choice < 7));
    switch (choice) {
        case 1:
            prod_add();
            break;
        case 2:
            prod_display();
            break;
        case 3:
            prod_sud_menu("search");
            break;
        case 4:
            prod_sud_menu("update");
            break;
        case 5:
            prod_sud_menu("delete");
            break;
    }
}
/**
 * @brief Teller Details Menu
 * 
 */
void teller_menu(void) {
    clrscr();
    int choice;
    printf("---------- Teller Details ----------\n\n");
    printf("[1] Add\n");
    printf("[2] Display\n");
    printf("[3] Search\n");
    printf("[4] Update\n");
    printf("[5] Delete\n");
    printf("[6] Go Back\n");
    printf("-------------------------------------\n\n");
    do {
        printf("Choice: ");
        dscanc(&choice);
        if (!(choice > 0 && choice < 7))
            printf("Invalid Choice!\n");
    } while (!(choice > 0 && choice < 7));
    switch (choice) {
        case 1:
            break;
        case 2:
            break;
        case 3:
            break;
        case 4:
            break;
        case 5:
            break;
        default:
    }
}
/**
 * @brief Sale Transaction Menu
 * 
 */
void sales_menu(void) {
    clrscr();
    int choice;
    printf("---------- Sale Transaction ----------\n\n");
    printf("[1] New Transaction\n");
    printf("[2] Display Transaction\n");
    printf("[3] Go Back\n");
    printf("--------------------------------------\n\n");
    do {
        printf("Choice: ");
        dscanc(&choice);
        if (!(choice > 0 && choice < 4))
            printf("Invalid Choice!\n");
    } while (!(choice > 0 && choice < 4));
    switch (choice) {
        case 1:
            break;
        case 2:
            break;
        default:
    }
}
/**
 * @brief Add new Product Details
 * 
 * @return int [0] if saved; else canceled / not saved;
 */
int prod_add(void) {
    clrscr();
    char save;
    Product product;
    memset(&product, 0, sizeof(product)); // set Product data to empty or 0
    printf("---------- Add Product Details ----------\n\n");
    printf("Product ID : ");
    // scanf("%d", &product.id);
    printf("Product Name : ");
    scanf("%[^\n]s", product.name); // %[^\n]s reads inputted text after newline or [Enter] character '\n'
    printf("Product Description : ");
    scanf("%[^\n]s", product.description);
    printf("Product Category : ");
    scanf("%[^\n]s", product.category);
    printf("Product Unit : ");
    scanf("%[^\n]s", product.unit);
    printf("Product Unit Price : ");
    scanf("%.2f", &product.unit); // %.2f reads inpputed floating number with 2 decimal places
    printf("\nSave these data?\nType 'y' if yes, 'n' if no:");
    do {
        if (save == 'N' || save == 'n')
            return -1; // cancelled / not saved
    } while (save != 'N' || save != 'n' || save != 'y' || save != 'Y');
    return 0;
}
/**
 * @brief Display All Product Details
 * 
 */
void prod_display(void) {

}
/**
 * @brief Product Search/Update/Delete Menu
 * 
 */
void prod_sud_menu(const char * request) {
    clrscr();
    int choice;
    printf("---------- %s Product Details ----------\n\n", capitalize(request));
    printf("[1] By Product ID\n");
    printf("[2] By Product Name\n");
    printf("[3] Go Back\n");
    printf("--------------------------------------------\n");
    printf("Choice: ");
    scanf("%d", &choice);
    switch (choice) {
        case 1:
            break;
        case 2:
            break;
        default:
            prod_menu();
    }
    getch();
}
/**
 * @brief Product Search/Update/Delete Request by ID
 * 
 * @param id Product ID search
 * @param request "search" | "update" | "delete" #(all other string characters are ignored)
 */
void prod_search_id(int id, const char * request) {

}
/**
 * @brief Product Search/Update/Delete Request by Product Name
 * 
 * @param prod_name Product Name search
 * @param request "search" | "update" | "delete" #(all other string characters are ignored)
 */
void prod_search_name(const char * prod_name, const char * request) {

}
/**
 * @brief Add new Teller Details
 * 
 * @return int [0] if saved; else canceled / not saved;
 */
int teller_add(void) {
    char save;
    Teller teller;
    memset(&teller, 0, sizeof(teller)); // set Teller data to empty or 0
    printf("---------- Add Teller Details ----------\n\n");
    printf("Product ID : ");
    // scanf("%d", &teller.id);
    printf("Product Name : ");
    scanf("%[^\n]s", teller.first_name);
    scanf("%[^\n]s", teller.middle_name);
    scanf("%[^\n]s", teller.last_name);
    printf("\nSave these data?\nType 'y' if yes, 'n' if no:");
    do {
        if (save == 'N' || save == 'n')
            return -1; // cancelled / not saved
    } while (save != 'N' || save != 'n' || save != 'y' || save != 'Y');
    return 0;
}
/**
 * @brief Display All Teller Details
 * 
 */
void teller_display(void) {

}
/**
 * @brief Teller Search/Update/Delete Menu
 * 
 */
void teller_sud_menu(const char * request) {

}
/**
 * @brief Teller Search/Update/Delete Request by ID
 * 
 * @param id Teller ID search
 * @param request "search" | "update" | "delete" #(all other string characters are ignored)
 */
void teller_search_id(int id, const char * request) {

}
/**
 * @brief Teller Search/Update/Delete Request by Teller Name
 * 
 * @param teller_name Teller Name search
 * @param request "search" | "update" | "delete" #(all other string characters are ignored)
 */
void teller_search_name(const char * prod_name, const char * request) {

}
/**
 * @brief 
 * 
 */
void sale_new(void) {
    printf("---------- Sale Transaction ----------\n\n");
    printf("[1] New Transaction\n");
    printf("[2] Display Transaction\n");
    printf("\n--------------------------------------\n");
}
/**
 * @brief Display all Sale Transactions
 * 
 */
void sale_display(void) {
    SaleTransaction sale;
    memset(&sale, 0, sizeof(sale)); // set SaleTransaction data to empty or 0
    memset(sale.items, 0, MAX_ITEMS*sizeof(sale.items[0])); // set ProductItem data to empty or 0
    char choice, i, count = 0, appendDisplay[5000];
    int searchID;
    memset(appendDisplay, 0, 5000*sizeof(appendDisplay[0])); // set appendDisplay to empty string
    printf("---------- New Transaction ----------\n");
    sprintf(appendDisplay, "---------- New Transaction ----------\n");
    do {
        memset(&sale.items[count].product, 0, sizeof(sale.items[count].product)); // set the Product data empty or 0
        printf("\nSale ID : ");
        // scanf("%d", &sale.id);
        sprintf(appendDisplay, "\nSale ID : %d\n");
        printf("Product ID : "); // We will use product ID...
        scanf("%d", &searchID); // ...rather than Product name for input to search the specific existing product
        // searching for product details by ID and put it in the SaleTransaction data
        while (getProductByID(&sale.items[count].product, searchID) != 0) { // loop while product id does not exists in records
            printf("Product not found!\n");
            getch();
            clrscr(); // clear the command line screen
            printf("%s", appendDisplay); // then display the previous successful add item and the current sale transaction ID
            printf("Product ID : ");
            scanf("%d", &searchID); // input product ID again
        }
        // append display to appendDisplay variable string of the selected product details     
        sprintf(appendDisplay, "Product Name : %s\n", sale.items[count].product.name);
        sprintf(appendDisplay, "Product Unit : %s\n", sale.items[count].product.unit);
        sprintf(appendDisplay, "Product Price : %.2f\n", sale.items[count].product.unit_price);
        sprintf(appendDisplay, "Quantity : %d\n", &sale.items[count].quantity);
        clrscr(); // clear the command line screen
        printf("%s", appendDisplay); // display previous and current product details of selected products
        count++; // increment the count of recorded sale transaction
        do {
            printf("Do you want to add another item?\n");
            printf("Type 'y' if yes, 'n' if no: ");
            cscanc(&choice);
            if (!(choice == 'n' || choice == 'N' || choice == 'y' || choice == 'Y'))
                printf("Invalid Choice!\n");
        } while (!(choice == 'n' || choice == 'N' || choice == 'y' || choice == 'Y'));
        // repeat if yes
    } while (!(choice == 'n' || choice == 'N'));
    clrscr(); // clear the command line screen
    // display total
    sprintf(appendDisplay, "_____________________________________\n");
    sprintf(appendDisplay, "Total Payable Amount:\t%.2f", compute_payable_amount(&sale));
    sprintf(appendDisplay, "Cash: ");
    printf("%s", appendDisplay);
    scanf("%.2f", &sale.cash);
    sprintf(appendDisplay, "%.2f\n", sale.cash);
    sprintf(appendDisplay, "\nChange: %.2f", compute_change(&sale));
    printf("\nChange: %.2f\n", sale.change);
    // TODO: write to txt file
    getch();
}
/**
 * @brief Compute payable amount of sales, record it to struct and return the amount
 * 
 * @param sale SaleTransaction structure data
 * @return float Total Payable Amount
 */
float compute_payable_amount(SaleTransaction * sale) {
    int i, total_items = 0;
    float sum = 0;
    total_items = sizeof(sale->items) / sizeof(sale->items[0]);
    for (i = 0; i < total_items; i++) {
        sum += sale->items[i].quantity * sale->items[i].product.unit_price;
    }
    sale->payable_amount = sum;
    return sum;
}
/**
 * @brief Get the Product data to buffer By ID
 * 
 * @param productbuffer 
 * @param id 
 * @return int 0 - successfully retrieved data; else - failed to retrieve data
 */
int getProductByID(Product * productbuffer, int id) {

    return 0;
}

/**
 * @brief Compute the change of total payable amount and cash given, record it to struct and return the change amount
 * 
 * @param sale SaleTransaction structure data
 * @return float Total Change Amount
 */
float compute_change(SaleTransaction * sale) {
    return (float)(sale->payable_amount - sale->cash);
}
// other functions
/**
 * @brief Custom Scanf for single character input for integer number
 * user single-input integer
 * @param d int buffer
 * @return int integer character inputted
 */
int dscanc(int * d) {
    char cbuf, buf[2];
    memset(&buf, 0, 2);
    cbuf = getch();
    *buf = cbuf;
    *d = atoi(buf);
    printf("%d\n", *d);
    return *d;
}
/**
 * @brief Custom Scanf for single character input for char
 * user single-input char
 * @param c char buffer
 * @return int character inputted
 */
int cscanc(char * c) {
    *c = getch();
    printf("%c\n", *c);
    return *c;
}
/**
 * @brief Capitalize the first letter of the word/string
 * 
 * @param word 
 * @return char* 
 */
char * capitalize(const char * word) {
    int upper = 0;
    char buf[250], *ret;
    memset(buf, 0, 250*sizeof(buf[0]));
    strcpy(buf, word);
    char * temp = (char *)buf;
    for (; *temp; ++temp) {
        if (upper == 0) {
            *temp = toupper((unsigned char)*temp); // Capitalize first letter
            upper++;
        } else
            *temp = tolower((unsigned char)*temp); // lowercase other letters
    }
    ret = (char *)buf;
    return ret;
}