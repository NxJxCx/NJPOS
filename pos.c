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
#include <time.h>
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
#define TIME_SIZE 50
#define MAX_NAME 250
#define MAX_ITEMS 2000
#define PRODUCTRECORDS "product_records.bin"
#define TELLERRECORDS "teller_records.bin"
#define SALERECORDS "sale_records.bin"
#define SALETRANSACTIONS "%s_sale_transaction.txt"

// Define Structures
typedef struct {
    int id; // id of product
    char name[MAX_NAME]; // name of product
    char description[MAX_NAME]; // description of product
    char category[MAX_NAME]; // category of product
    char unit[MAX_NAME]; // unit of product
    float unit_price; // unit price of product
} Product; // Product Details
typedef struct {
    int id; // Teller id
    char first_name[MAX_NAME]; // first name of teller
    char middle_name[MAX_NAME]; // middle name of teller
    char last_name[MAX_NAME]; // last name of teller
} Teller; // Teller Details
typedef struct {
    int id; // sale id
    Product product; // product item
    int quantity; // quantity of product item
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
int prod_search_id(int id, const char * request); // Product Search/Update/Delete Request by ID
int prod_search_name(const char * prod_name, const char * request); // Product Search/Update/Delete Request by Product Name
int teller_add(void); // Add new Teller Details
void teller_display(void); // Display all Teller Details
void teller_sud_menu(const char * request); // Teller Search/Update/Delete Menu
int teller_search_id(int id, const char * request); // Teller Search/Update/Delete Request by ID
int teller_search_name(const char * teller_name, const char * request); // Teller Search/Update/Delete Request by Product Name
void sale_new(void); // add new transaction
void sale_display(void); // Display Transactions
float compute_payable_amount(SaleTransaction * sale, int count); // Compute Total Payable amount
float compute_change(float payable_amount, float cash); // Compute Total Payable amount
int getRecordCount(const char * filename, int recordsize); // Get record count from file
int getLatestID(const char * filename, int recordsize); // get the latest maximum ID from file
int saveProductToFile(Product * product, int count); // save product to file
int saveTellerToFile(Teller * teller, int count); // save teller to file
int saveSaleTransactionToFile(SaleTransaction * sale, int count); // save sale transaction to file
void getProductData(Product * product); // get product data from file
void getTellerData(Teller * teller); // get teller data from file
void getSaleData(SaleTransaction * sale); // get sale transaction data from file
int getProductByID(Product * productbuffer, int searchID); // get Product struct by search ID
// other function prototypes
int dscanc(int * d); // user single-input integer
int cscanc(char * c); // user single-input char
char * capitalize(const char * word); // Capitalize first letter of the word/string
int maxOfInt(int * arr, int count); // get the maximum value of an int array
char * centerTheString(const char * strval, int size); // center the string content by size of char
char * rightAlignFloat(float value, int charsize); // right align as string from floating value in specific char size
void customScanfDefaultString(char * buffer, const char * defaultVal);
void customScanfDefaultFloat(float * buffer, float defaultVal);
void customScanfDefaultInt(int * buffer, int defaultVal);


int main(int argc, char * argv[]) {
    FILE * fp; // create binary files if not exists
    if ((fp = fopen(PRODUCTRECORDS, "ab")) == NULL)
        exit(1);
    fclose(fp);
    if ((fp = fopen(TELLERRECORDS, "ab")) == NULL)
        exit(1);
    fclose(fp);
    if ((fp = fopen(SALERECORDS, "ab")) == NULL)
        exit(1);
    fclose(fp);
    while (1) {
        if (CLI() == 4) // 4 = exit
            break;
    }
    printf(" => Thank you for using this Point of Sales (POS) System. Goodbye!");
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
    printf("\n ---------- Welcome To NJC Enterprise ----------\n");
    printf(" Please select:\n\n");
    printf(" [1] Product Details\n");
    printf(" [2] Teller Details\n");
    printf(" [3] Sale Transaction\n");
    printf(" [4] Exit Program\n");
    printf("\n ----------------------------------------------\n\n");
    do {
        printf(" Enter Choice: ");
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
    printf("\n ---------- Product Details ----------\n\n");
    printf(" [1] Add\n");
    printf(" [2] Display\n");
    printf(" [3] Search\n");
    printf(" [4] Update\n");
    printf(" [5] Delete\n");
    printf(" [6] Go Back\n");
    printf("\n -------------------------------------\n\n");
    do {
        printf(" Choice: ");
        dscanc(&choice);
        if (!(choice > 0 && choice < 7))
            printf(" Invalid Choice!\n");
    } while (!(choice > 0 && choice < 7));
    switch (choice) {
        case 1:
            char cnew;
            while (prod_add() == 0) {
                printf(" Add new product?\n");
                do {
                    printf(" Type 'y' if yes, 'n' if no: ");
                    cscanc(&cnew);
                } while (!(cnew == 'y' || cnew == 'Y' || cnew == 'n' || cnew == 'N'));
                if (cnew == 'n' || cnew == 'N')
                    break;
            }
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
    printf("\n ---------- Teller Details ----------\n\n");
    printf(" [1] Add\n");
    printf(" [2] Display\n");
    printf(" [3] Search\n");
    printf(" [4] Update\n");
    printf(" [5] Delete\n");
    printf(" [6] Go Back\n");
    printf("\n -------------------------------------\n\n");
    do {
        printf(" Choice: ");
        dscanc(&choice);
        if (!(choice > 0 && choice < 7))
            printf(" Invalid Choice!\n");
    } while (!(choice > 0 && choice < 7));
    switch (choice) {
        case 1:
            char cnew;
            while (teller_add() == 0) {
                printf(" Add new teller?\n");
                do {
                    printf(" Type 'y' if yes, 'n' if no: ");
                    cscanc(&cnew);
                } while (!(cnew == 'y' || cnew == 'Y' || cnew == 'n' || cnew == 'N'));
                if (cnew == 'n' || cnew == 'N')
                    break;
            }
            break;
        case 2:
            teller_display();
            break;
        case 3:
            teller_sud_menu("search");
            break;
        case 4:
            teller_sud_menu("update");
            break;
        case 5:
            teller_sud_menu("delete");
            break;
    }
}
/**
 * @brief Sale Transaction Menu
 * 
 */
void sales_menu(void) {
    clrscr();
    int choice;
    printf("\n ---------- Sale Transaction ----------\n\n");
    printf(" [1] New Transaction\n");
    printf(" [2] Display Transaction\n");
    printf(" [3] Go Back\n");
    printf("\n --------------------------------------\n\n");
    do {
        printf(" Choice: ");
        dscanc(&choice);
        if (!(choice > 0 && choice < 4))
            printf(" Invalid Choice!\n");
    } while (!(choice > 0 && choice < 4));
    switch (choice) {
        case 1:
            sale_new();
            break;
        case 2:
            sale_display();
            break;
    }
}
/**
 * @brief Add new Product Details
 * 
 * @return int [0] if saved; else canceled / not saved;
 */
int prod_add(void) {
    clrscr();
    int count, index;
    char save;
    index = getRecordCount(PRODUCTRECORDS, sizeof(Product));
    count = index + 1;
    Product product[count];
    if (index == 0) { // if record has no records at all
        memset(&product[0], 0, sizeof(Product));
    } else { // if record has records
        memset(product, 0, sizeof(product));
        getProductData(product);
    }
    fflush(stdin);
    product[index].id = getLatestID(PRODUCTRECORDS, sizeof(Product)); // set product id + 1 to the latest id
    product[index].id++;
    printf("\n ---------- Add Product Details ----------\n\n");
    printf(" Product ID : %d\n", product[index].id);
    printf(" Product Name : ");
    fflush(stdin);
    scanf("%[^\n]s", product[index].name); // %[^\n]s reads inputted text after newline or [Enter] character '\n'
    fflush(stdin); // we need fflush because we use %[^\n]s format
    printf(" Product Description : ");
    scanf("%[^\n]s", product[index].description);
    fflush(stdin); // we need fflush because we use %[^\n]s format
    printf(" Product Category : ");
    scanf("%[^\n]s", product[index].category);
    fflush(stdin); // we need fflush because we use %[^\n]s format
    printf(" Product Unit : ");
    scanf("%[^\n]s", product[index].unit);
    fflush(stdin); // we need fflush because we use %[^\n]s format
    do {
        printf(" Product Unit Price : ");
        customScanfDefaultFloat(&product[index].unit_price, -1.0);
    } while (product[index].unit_price < 0.0);// if inputted amount is invalid, repeat input
    printf("\n Save these data?\n");
    do {
        printf(" Type 'y' if yes, 'n' if no: ");
        cscanc(&save);
        if (save == 'N' || save == 'n')
            return -1; // cancelled / not saved
    } while (!(save == 'y' || save == 'Y'));
    if (0 == saveProductToFile(product, count))
        printf("\n => Product added successfully!\n\n");
    else
        printf("\n => ERROR WRITING TO FILE. Product add failed.");
    return 0;
}
/**
 * @brief Display All Product Details
 * 
 */
void prod_display(void) {
    clrscr();
    int i, count = 0;
    char name[20], desc[20], cat[20], p_unit[20], p_price[15]; 
    count = getRecordCount(PRODUCTRECORDS, sizeof(Product));
    Product products[count];
    if (count < 1)
        goto displayEmptyResults; // redirect to empty records
    getProductData(products); // get product data
    printf("\n ---------- Display Product Details ----------\n\n");
    printf(" %s%s%s%s%s%s\n\n", "Product ID", "    Product Name    ", "Product Description ", "  Product Category  ", "    Product Unit    ", " Product Unit Price ");
    for (i = 0; i < count; i++) {
        strcpy(name, centerTheString(products[i].name, sizeof(name)));
        strcpy(desc, centerTheString(products[i].description, sizeof(desc)));
        strcpy(cat, centerTheString(products[i].category, sizeof(cat)));
        strcpy(p_unit, centerTheString(products[i].unit, sizeof(p_unit)));
        // display data
        printf("  %08d %s%s%s%s", products[i].id, name, desc, cat, p_unit);
        strcpy(p_price, rightAlignFloat(products[i].unit_price, sizeof(p_price)));
        printf("%s\n", p_price);
    }
    printf("\n ---------------------------------------------\n\n");
    getch();
    return; // end of display
    displayEmptyResults: // label for displaying empty product details
        printf("\n ---------- Display Product Details ----------\n\n");
        printf(" %s%s%s%s%s%s\n\n", "Product ID", "    Product Name    ", "Product Description ", "  Product Category  ", "    Product Unit    ", " Product Unit Price ");
        printf("\n ---------------------------------------------\n\n");
        getch();
}
/**
 * @brief Product Search/Update/Delete Menu
 * 
 */
void prod_sud_menu(const char * request) {
    clrscr();
    int choice;
    printf("\n ---------- %s Product Details ----------\n\n", capitalize(request));
    printf(" [1] By Product ID\n");
    printf(" [2] By Product Name\n");
    printf(" [3] Go Back\n");
    printf("\n ----------------------------------------------\n");
    do {
        printf(" Choice: ");
        dscanc(&choice);
        if (!(choice > 0 && choice < 4))
            printf(" Invalid Choice!\n");
    } while (!(choice > 0 && choice < 4));
    switch (choice) {
        case 1:
            int id;
            fflush(stdin);
            do {
                printf("\nEnter ID: ");
                scanf("%d", &id);
                fflush(stdin);
            } while(prod_search_id(id, request) != 0);
            break;
        case 2:
            char namesearch[MAX_NAME];
            fflush(stdin);
            do {
                memset(namesearch, 0, MAX_NAME);
                strcpy(namesearch, "");
                printf("\nEnter Product Name: ");
                scanf("%[^\n]s", &namesearch);
                fflush(stdin);
            } while(prod_search_name(namesearch, request) != 0);
            break;
        default:
            prod_menu();
    }
}
/**
 * @brief Product Search/Update/Delete Request by ID
 * 
 * @param id Product ID search
 * @param request "search" | "update" | "delete" #(all other string characters are ignored)
 * @return int 0 - done with search | else still searching
 */
int prod_search_id(int id, const char * request) {
    clrscr();
    int i, count, selectedIndex = -1;
    char endchoice, name[20], desc[20], cat[20], p_unit[20], p_price[15];
    count = getRecordCount(PRODUCTRECORDS, sizeof(Product));
    Product products[count], productSelected; // array of products with count
    memset(products, 0, sizeof(products));
    memset(&productSelected, 0, sizeof(productSelected));
    if (count < 1)
        goto NoRecords; // no records found since count is 0
    getProductData(products); // read from file to products struct
    printf("\n ---------- %s Product Details ----------\n\n", capitalize(request));
    printf(" %s%s%s%s%s%s\n\n", "Product ID", "    Product Name    ", "Product Description ", "  Product Category  ", "    Product Unit    ", " Product Unit Price ");
    for (i=0; i < count; i++) {
        if (products[i].id == id) {
            selectedIndex = i;
            // copy to selected
            productSelected.id = products[selectedIndex].id;
            strcpy(productSelected.name, products[selectedIndex].name);
            strcpy(productSelected.description, products[selectedIndex].description);
            strcpy(productSelected.category, products[selectedIndex].category);
            strcpy(productSelected.unit, products[selectedIndex].unit);
            productSelected.unit_price = products[selectedIndex].unit_price;
            // copy to display
            strcpy(name, centerTheString(products[i].name, sizeof(name)));
            strcpy(desc, centerTheString(products[i].description, sizeof(desc)));
            strcpy(cat, centerTheString(products[i].category, sizeof(cat)));
            strcpy(p_unit, centerTheString(products[i].unit, sizeof(p_unit)));
            // display data
            printf("  %08d %s%s%s%s", products[i].id, name, desc, cat, p_unit);
            strcpy(p_price, rightAlignFloat(products[i].unit_price, sizeof(p_price)));
            printf("%s\n", p_price);
            goto Found; // redirect to Found label
        }
    }
    NotFound:
        goto EndNoRecTable; // no records found redirect
    Found:
        printf("\n --------------------------------------------\n\n");
        // Search / Update / Delete Functions
        if (0 == strcmp(request, "update")) {
            char savedata;
            Product oldData;
            memset(&oldData, 0, sizeof(Product));
            oldData.id = productSelected.id;
            strcpy(oldData.name, productSelected.name);
            strcpy(oldData.description, productSelected.description);
            strcpy(oldData.category, productSelected.category);
            strcpy(oldData.unit, productSelected.unit);
            oldData.unit_price = products[selectedIndex].unit_price;
            fflush(stdin);
            // update selected data
            printf(" => Update Data:\n");
            printf(" Product ID : %08d\n", oldData.id);
            printf(" Product Name (%s): ", oldData.name);
            customScanfDefaultString(productSelected.name, oldData.name);
            printf(" Product Description (%s): ", oldData.description);
            customScanfDefaultString(productSelected.description, oldData.description);
            printf(" Product Category (%s): ", oldData.category);
            customScanfDefaultString(productSelected.category, oldData.category);
            printf(" Product Unit (%s): ", oldData.unit);
            customScanfDefaultString(productSelected.unit, oldData.unit);
            printf(" Product Unit Price (%.2f): ", oldData.unit_price);
            customScanfDefaultFloat(&productSelected.unit_price, oldData.unit_price);
            printf("\n Save modified data?\n");
            do {
                printf(" Type 'y' if yes, 'n' if no: ");
                cscanc(&savedata);
                if (savedata == 'N' || savedata == 'n') {
                    goto SearchAgain; // cancelled / not saved
                }
            } while (!(savedata == 'y' || savedata == 'Y' || savedata == 'n' || savedata == 'N'));
            // copy modified data to all products selected index
            products[selectedIndex].id = productSelected.id;
            strcpy(products[selectedIndex].name, productSelected.name);
            strcpy(products[selectedIndex].description, productSelected.description);
            strcpy(products[selectedIndex].category, productSelected.category);
            strcpy(products[selectedIndex].unit, productSelected.unit);
            products[selectedIndex].unit_price = productSelected.unit_price;
            // save to file
            if (0 != saveProductToFile(products, count)) {
                printf(" Something went wrong. Try again.\n\n");
                goto SearchAgain;
            }
            printf("\n ==> Successfully Updated Record to file!\n\n");
        } else if (0 == strcmp(request, "delete")) {
            // delete selected data
            char dchoice;
            printf(" => Do you really want to delete this record?\n");
            do {
                printf(" Type 'y' if yes, 'n' if no: ");
                cscanc(&dchoice);
                if (dchoice == 'n' || dchoice == 'N')
                    goto SearchAgain;
            } while (!(dchoice == 'y' || dchoice == 'Y' || dchoice == 'n' || dchoice == 'N'));
            printf(" ==> Deleting Record...\n");
            int newCount = 0, j = 0;
            Product productsNew[count];
            memset(productsNew, 0, sizeof(productsNew));
            for (i = 0; i < count; i++) {
                if (products[i].id == id)
                    continue; // skip loop if selectedIndex is equal to i to be deleted
                // else copy the record/row to productsNew[i]
                productsNew[j].id = products[i].id;
                strcpy(productsNew[j].name, products[i].name);
                strcpy(productsNew[j].description, products[i].description);
                strcpy(productsNew[j].category, products[i].category);
                strcpy(productsNew[j].unit, products[i].unit);
                productsNew[j].unit_price = products[i].unit_price;
                j++; // j for productsNew index
                newCount++; // new count
            }
            if (0 != saveProductToFile(productsNew, newCount)) {
                printf(" Something went wrong. Try again.\n\n");
                goto SearchAgain;
            }
            printf(" ==> Successfully Deleted Record from file!\n\n");
        }
        goto SearchAgain;
    EndRec:
        return 0;
    NoRecords:
        printf("\n ---------- %s Product Details ----------\n\n", capitalize(request));
    EndNoRecTable:
        printf(" => No Records found\n");
        printf("\n ---------------------------------------------\n\n");
    SearchAgain:
        do {
            printf(" Do you want to search again?\n");
            printf(" Type 'y' if yes, 'n' if no: ");
            cscanc(&endchoice);
            if (endchoice == 'n' || endchoice == 'N')
                goto EndRec; // return 0;
        } while(!(endchoice == 'y' || endchoice == 'Y' || endchoice == 'n' || endchoice == 'N'));
    EndNoRec:
        return -1;
}
/**
 * @brief Product Search/Update/Delete Request by Product Name
 * 
 * @param prod_name Product Name search
 * @param request "search" | "update" | "delete" #(all other string characters are ignored)
 * @return int 0 - done with search | else still searching
 */
int prod_search_name(const char * prod_name, const char * request) {
    clrscr();
    int i, k, l = 0, count, selectedIndex = -1, recordsCount = 0, selectedID = -1;
    char endchoice, name[20], desc[20], cat[20], p_unit[20], p_price[15];
    count = getRecordCount(PRODUCTRECORDS, sizeof(Product));
    int selectedIndexes[count];
    memset(selectedIndexes, 0, sizeof(selectedIndexes));
    Product products[count], productSelected[count], selectedProduct; // array of products with count
    memset(products, 0, sizeof(products));
    memset(productSelected, 0, sizeof(productSelected));
    memset(&selectedProduct, 0, sizeof(selectedProduct));
    if (count < 1)
        goto NoRecords; // no records found since count is 0
    getProductData(products); // read from file to products struct
    printf("\n ---------- %s Product Details ----------\n\n", capitalize(request));
    printf(" %s%s%s%s%s%s\n\n", "Product ID", "    Product Name    ", "Product Description ", "  Product Category  ", "    Product Unit    ", " Product Unit Price ");
    for (i=0; i < count; i++) {
        for (k = 0; k < strlen(products[i].name); k++) {
            if (0 == strnicmp(products[i].name + k, prod_name, strlen(prod_name))) {
                // copy to selected
                productSelected[l].id = products[i].id;
                strcpy(productSelected[l].name, products[i].name);
                strcpy(productSelected[l].description, products[i].description);
                strcpy(productSelected[l].category, products[i].category);
                strcpy(productSelected[l].unit, products[i].unit);
                productSelected[l].unit_price = products[i].unit_price;
                // copy index i to selectedIndexes[l]
                selectedIndexes[l] = i;
                // copy to display
                strcpy(name, centerTheString(products[i].name, sizeof(name)));
                strcpy(desc, centerTheString(products[i].description, sizeof(desc)));
                strcpy(cat, centerTheString(products[i].category, sizeof(cat)));
                strcpy(p_unit, centerTheString(products[i].unit, sizeof(p_unit)));
                // display data
                printf("  %08d %s%s%s%s", products[i].id, name, desc, cat, p_unit);
                strcpy(p_price, rightAlignFloat(products[i].unit_price, sizeof(p_price)));
                printf("%s\n", p_price);
                recordsCount++;
                l++; // l for productSelected index
                break; // break for k loop since name has been found
            }
        }
    }
    if (recordsCount > 0)
        goto Found;
    NotFound:
        goto EndNoRecTable; // no records found redirect
    Found:
        printf("\n --------------------------------------------\n\n");
        // Search / Update / Delete Functions
        if (0 == strcmp(request, "update")) {
            if (recordsCount > 1) {
                // make selection
                printf(" => Select one (1) row data to UPDATE:\n");
                do {
                    printf(" Enter ID: ");
                    customScanfDefaultInt(&selectedID, -1);
                    for (i = 0; i < l; i++) {
                        if (productSelected[i].id == selectedID) {
                            selectedIndex = selectedIndexes[i];
                            selectedProduct.id = productSelected[i].id;
                            strcpy(selectedProduct.name, productSelected[i].name);
                            strcpy(selectedProduct.description, productSelected[i].description);
                            strcpy(selectedProduct.category, productSelected[i].category);
                            strcpy(selectedProduct.unit, productSelected[i].unit);
                            selectedProduct.unit_price = productSelected[i].unit_price;
                            goto IDSelectedUpdate;
                        }
                    }
                    selectedID = -1;
                    printf(" ID not in selection! Please Try Again.\n");
                } while(selectedID < 0);
            } else {
                selectedIndex = selectedIndexes[0];
                selectedProduct.id = productSelected[0].id;
                strcpy(selectedProduct.name, productSelected[0].name);
                strcpy(selectedProduct.description, productSelected[0].description);
                strcpy(selectedProduct.category, productSelected[0].category);
                strcpy(selectedProduct.unit, productSelected[0].unit);
                selectedProduct.unit_price = productSelected[0].unit_price;
            }
            IDSelectedUpdate: // ID has selected label
                char savedata;
                Product oldData;
                memset(&oldData, 0, sizeof(Product));
                oldData.id = selectedProduct.id;
                strcpy(oldData.name, selectedProduct.name);
                strcpy(oldData.description, selectedProduct.description);
                strcpy(oldData.category, selectedProduct.category);
                strcpy(oldData.unit, selectedProduct.unit);
                oldData.unit_price = products[selectedIndex].unit_price;
                fflush(stdin);
                // update selected data
                printf(" => Update Data:\n");
                printf(" Product ID : %08d\n", oldData.id);
                printf(" Product Name (%s): ", oldData.name);
                customScanfDefaultString(selectedProduct.name, oldData.name);
                printf(" Product Description (%s): ", oldData.description);
                customScanfDefaultString(selectedProduct.description, oldData.description);
                printf(" Product Category (%s): ", oldData.category);
                customScanfDefaultString(selectedProduct.category, oldData.category);
                printf(" Product Unit (%s): ", oldData.unit);
                customScanfDefaultString(selectedProduct.unit, oldData.unit);
                printf(" Product Unit Price (%.2f): ", oldData.unit_price);
                customScanfDefaultFloat(&selectedProduct.unit_price, oldData.unit_price);
                printf("\n Save modified data?\n");
                do {
                    printf(" Type 'y' if yes, 'n' if no: ");
                    cscanc(&savedata);
                    if (savedata == 'N' || savedata == 'n') {
                        goto SearchAgain; // cancelled / not saved
                    }
                } while (!(savedata == 'y' || savedata == 'Y' || savedata == 'n' || savedata == 'N'));
                // copy modified data to all products selected index
                products[selectedIndex].id = selectedProduct.id;
                strcpy(products[selectedIndex].name, selectedProduct.name);
                strcpy(products[selectedIndex].description, selectedProduct.description);
                strcpy(products[selectedIndex].category, selectedProduct.category);
                strcpy(products[selectedIndex].unit, selectedProduct.unit);
                products[selectedIndex].unit_price = selectedProduct.unit_price;
                // save to file
                if (0 != saveProductToFile(products, count)) {
                    printf(" Something went wrong. Try again.\n\n");
                    goto SearchAgain;
                }
                printf("\n ==> Successfully Updated Record to file!\n\n");
        } else if (0 == strcmp(request, "delete")) {
            // delete selected data
            if (recordsCount > 1) {
                // make selection
                printf(" => Select one (1) row data to DELETE:\n");
                do {
                    printf(" Enter ID: ");
                    customScanfDefaultInt(&selectedID, -1);
                    for (i = 0; i < l; i++) {
                        if (productSelected[i].id == selectedID)
                            goto IDSelectedDelete;
                    }
                    selectedID = -1;
                    printf(" ID not in selection! Please Try Again.\n");
                } while(selectedID < 0);
            } else
                selectedID = productSelected[0].id;
            IDSelectedDelete: // ID has selected label
                char dchoice;
                printf(" Selected ID: %08d\n", selectedID);
                printf(" => Do you really want to delete ID # %08d record?\n", selectedID);
                do {
                    printf(" Type 'y' if yes, 'n' if no: ");
                    cscanc(&dchoice);
                    if (dchoice == 'n' || dchoice == 'N')
                        goto SearchAgain;
                } while (!(dchoice == 'y' || dchoice == 'Y' || dchoice == 'n' || dchoice == 'N'));
                printf(" ==> Deleting Record...\n");
                int newCount = 0, j = 0;
                Product productsNew[count];
                memset(productsNew, 0, sizeof(productsNew));
                for (i = 0; i < count; i++) {
                    if (products[i].id == selectedID)
                        continue; // skip loop if selectedIndex is equal to i to be deleted
                    // else copy the record/row to productsNew[i]
                    productsNew[j].id = products[i].id;
                    strcpy(productsNew[j].name, products[i].name);
                    strcpy(productsNew[j].description, products[i].description);
                    strcpy(productsNew[j].category, products[i].category);
                    strcpy(productsNew[j].unit, products[i].unit);
                    productsNew[j].unit_price = products[i].unit_price;
                    j++; // j for productsNew index
                    newCount++; // new count
                }
                if (0 != saveProductToFile(productsNew, newCount)) {
                    printf(" Something went wrong. Try again.\n\n");
                    goto SearchAgain;
                }
                printf(" ==> Successfully Deleted Record from file!\n\n");
        }
        goto SearchAgain;
    EndRec:
        return 0;
    NoRecords:
        printf("\n ---------- %s Product Details ----------\n\n", capitalize(request));
    EndNoRecTable:
        printf(" => No Records found\n");
        printf("\n ---------------------------------------------\n\n");
    SearchAgain:
        do {
            printf(" Do you want to search again?\n");
            printf(" Type 'y' if yes, 'n' if no: ");
            cscanc(&endchoice);
            if (endchoice == 'n' || endchoice == 'N')
                goto EndRec; // return 0;
        } while(!(endchoice == 'y' || endchoice == 'Y' || endchoice == 'n' || endchoice == 'N'));
    EndNoRec:
        return -1;
}
/**
 * @brief Add new Teller Details
 * 
 * @return int [0] if saved; else canceled / not saved;
 */
int teller_add(void) {
    clrscr();
    int count, index;
    char save;
    index = getRecordCount(TELLERRECORDS, sizeof(Teller));
    count = index + 1;
    Teller teller[count];
    if (index == 0) { // if record has no records at all
        memset(&teller[0], 0, sizeof(Teller));
    } else { // if record has records
        memset(teller, 0, sizeof(teller));
        getTellerData(teller);
    }
    fflush(stdin);
    teller[index].id = getLatestID(TELLERRECORDS, sizeof(Teller)); // set teller id + 1 to the latest id
    teller[index].id++;
    printf("\n ---------- Add Teller Details ----------\n\n");
    printf(" Teller ID : %d\n", teller[index].id);
    printf(" Teller First Name : ");
    fflush(stdin);
    scanf("%[^\n]s", teller[index].first_name); // %[^\n]s reads inputted text after newline or [Enter] character '\n'
    fflush(stdin); // we need fflush because we use %[^\n]s format
    printf(" Teller Middle Name : ");
    scanf("%[^\n]s", teller[index].middle_name);
    fflush(stdin); // we need fflush because we use %[^\n]s format
    printf(" Teller Last Name : ");
    scanf("%[^\n]s", teller[index].last_name);
    fflush(stdin); // we need fflush because we use %[^\n]s format
    printf("\n Save these data?\n");
    do {
        printf(" Type 'y' if yes, 'n' if no: ");
        cscanc(&save);
        if (save == 'N' || save == 'n')
            return -1; // cancelled / not saved
    } while (!(save == 'y' || save == 'Y'));
    if (0 == saveTellerToFile(teller, count))
        printf("\n => Teller details added successfully!\n\n");
    else
        printf("\n => ERROR WRITING TO FILE. Teller details add failed.");
    return 0;
}
/**
 * @brief Display All Teller Details
 * 
 */
void teller_display(void) {
    clrscr();
    int i, count = 0;
    char first_name[26], middle_name[26], last_name[26];
    count = getRecordCount(TELLERRECORDS, sizeof(Teller));
    Teller tellers[count];
    if (count < 1)
        goto displayEmptyResults; // redirect to empty records
    getTellerData(tellers); // get teller records
    printf("\n ---------- Display Teller Details ----------\n\n");
    printf(" %s%s%s%s\n\n", "Teller ID", "     Teller First Name    ", "    Teller Middle Name    ", "     Teller Last Name     ");
    for (i = 0; i < count; i++) {
        strcpy(first_name, centerTheString(tellers[i].first_name, sizeof(first_name)));
        strcpy(middle_name, centerTheString(tellers[i].middle_name, sizeof(middle_name)));
        strcpy(last_name, centerTheString(tellers[i].last_name, sizeof(last_name)));
        // display data
        printf("  %08d %s%s%s\n", tellers[i].id, first_name, middle_name, last_name);
    }
    printf("\n --------------------------------------------\n\n");
    getch();
    return; // end of display
    displayEmptyResults: // label for displaying empty teller details
        printf("\n ---------- Display Teller Details ----------\n\n");
        printf(" %s%s%s%s\n\n", "Teller ID", "     Teller First Name    ", "    Teller Middle Name    ", "     Teller Last Name     ");
        printf("\n --------------------------------------------\n\n");
        getch();
}
/**
 * @brief Teller Search/Update/Delete Menu
 * 
 */
void teller_sud_menu(const char * request) {
    clrscr();
    int choice;
    printf("\n ---------- %s Teller Details ----------\n\n", capitalize(request));
    printf(" [1] By Teller ID\n");
    printf(" [2] By Teller Name\n");
    printf(" [3] Go Back\n");
    printf("\n ---------------------------------------------\n");
    printf("Choice: ");
    do {
        printf(" Choice: ");
        dscanc(&choice);
        if (!(choice > 0 && choice < 4))
            printf(" Invalid Choice!\n");
    } while (!(choice > 0 && choice < 4));
    switch (choice) {
        case 1:
            int id;
            fflush(stdin);
            do {
                printf("\nEnter ID: ");
                scanf("%d", &id);
                fflush(stdin);
            } while(teller_search_id(id, request) != 0);
            break;
        case 2:
            char namesearch[MAX_NAME];
            fflush(stdin);
            do {
                memset(namesearch, 0, MAX_NAME);
                strcpy(namesearch, "");
                printf("\nEnter Teller Name: ");
                scanf("%[^\n]s", &namesearch);
                fflush(stdin);
            } while(teller_search_name(namesearch, request) != 0);
            break;
        default:
            teller_menu();
    }
}
/**
 * @brief Teller Search/Update/Delete Request by ID
 * 
 * @param id Teller ID search
 * @param request "search" | "update" | "delete" #(all other string characters are ignored)
 * @return int 0 - done with search | else still searching
 */
int teller_search_id(int id, const char * request) {
    clrscr();
    int i, count, selectedIndex = -1;
    char endchoice, first_name[26], middle_name[26], last_name[26];
    count = getRecordCount(TELLERRECORDS, sizeof(Teller));
    Teller tellers[count], tellerSelected; // array of tellers with count
    memset(tellers, 0, sizeof(tellers));
    memset(&tellerSelected, 0, sizeof(tellerSelected));
    if (count < 1)
        goto NoRecords; // no records found since count is 0
    getTellerData(tellers); // read from file to tellers struct
    printf("\n ---------- %s Teller Details ----------\n\n", capitalize(request));
    printf(" %s%s%s%s\n\n", "Teller ID", "     Teller First Name    ", "    Teller Middle Name    ", "     Teller Last Name     ");
    for (i=0; i < count; i++) {
        if (tellers[i].id == id) {
            selectedIndex = i;
            // copy to selected
            tellerSelected.id = tellers[selectedIndex].id;
            strcpy(tellerSelected.first_name, tellers[selectedIndex].first_name);
            strcpy(tellerSelected.middle_name, tellers[selectedIndex].middle_name);
            strcpy(tellerSelected.last_name, tellers[selectedIndex].last_name);
            // copy to display
            strcpy(first_name, centerTheString(tellers[i].first_name, sizeof(first_name)));
            strcpy(middle_name, centerTheString(tellers[i].middle_name, sizeof(middle_name)));
            strcpy(last_name, centerTheString(tellers[i].last_name, sizeof(last_name)));
            // display data
            printf("  %08d %s%s%s\n", tellers[i].id, first_name, middle_name, last_name);
            goto Found; // redirect to Found label
        }
    }
    NotFound:
        goto EndNoRecTable; // no records found redirect
    Found:
        printf("\n ---------------------------------------------\n\n");
        // Search / Update / Delete Functions
        if (0 == strcmp(request, "update")) {
            char savedata;
            Teller oldData;
            memset(&oldData, 0, sizeof(Teller));
            oldData.id = tellerSelected.id;
            strcpy(oldData.first_name, tellerSelected.first_name);
            strcpy(oldData.middle_name, tellerSelected.middle_name);
            strcpy(oldData.last_name, tellerSelected.last_name);
            fflush(stdin);
            // update selected data
            printf(" => Update Data:\n");
            printf(" Teller ID : %08d\n", oldData.id);
            printf(" Teller First Name (%s): ", oldData.first_name);
            customScanfDefaultString(tellerSelected.first_name, oldData.first_name);
            printf(" Teller Middle Name (%s): ", oldData.middle_name);
            customScanfDefaultString(tellerSelected.middle_name, oldData.middle_name);
            printf(" Teller Last Name (%s): ", oldData.last_name);
            customScanfDefaultString(tellerSelected.last_name, oldData.last_name);
            printf("\n Save modified data?\n");
            do {
                printf(" Type 'y' if yes, 'n' if no: ");
                cscanc(&savedata);
                if (savedata == 'N' || savedata == 'n') {
                    goto SearchAgain; // cancelled / not saved
                }
            } while (!(savedata == 'y' || savedata == 'Y' || savedata == 'n' || savedata == 'N'));
            // copy modified data to all tellers selected index
            tellers[selectedIndex].id = tellerSelected.id;
            strcpy(tellers[selectedIndex].first_name, tellerSelected.first_name);
            strcpy(tellers[selectedIndex].middle_name, tellerSelected.middle_name);
            strcpy(tellers[selectedIndex].last_name, tellerSelected.last_name);
            // save to file
            if (0 != saveTellerToFile(tellers, count)) {
                printf(" Something went wrong. Try again.\n\n");
                goto SearchAgain;
            }
            printf("\n ==> Successfully Updated Record to file!\n\n");
        } else if (0 == strcmp(request, "delete")) {
            // delete selected data
            char dchoice;
            printf(" => Do you really want to delete this record?\n");
            do {
                printf(" Type 'y' if yes, 'n' if no: ");
                cscanc(&dchoice);
                if (dchoice == 'n' || dchoice == 'N')
                    goto SearchAgain;
            } while (!(dchoice == 'y' || dchoice == 'Y' || dchoice == 'n' || dchoice == 'N'));
            printf(" ==> Deleting Record...\n");
            int newCount = 0, j = 0;
            Teller tellersNew[count];
            memset(tellersNew, 0, sizeof(tellersNew));
            for (i = 0; i < count; i++) {
                if (tellers[i].id == id)
                    continue; // skip loop if selectedIndex is equal to i to be deleted
                // else copy the record/row to tellersNew[i]
                tellersNew[j].id = tellers[i].id;
                strcpy(tellersNew[j].first_name, tellers[i].first_name);
                strcpy(tellersNew[j].middle_name, tellers[i].middle_name);
                strcpy(tellersNew[j].last_name, tellers[i].last_name);
                j++; // j for tellersNew index
                newCount++; // new count
            }
            if (0 != saveTellerToFile(tellersNew, newCount)) {
                printf(" Something went wrong. Try again.\n\n");
                goto SearchAgain;
            }
            printf(" ==> Successfully Deleted Record from file!\n\n");
        }
        goto SearchAgain;
    EndRec:
        return 0;
    NoRecords:
        printf("\n ---------- %s Teller Details ----------\n\n", capitalize(request));
    EndNoRecTable:
        printf(" => No Records found\n");
        printf("\n ---------------------------------------------\n\n");
    SearchAgain:
        do {
            printf(" Do you want to search again?\n");
            printf(" Type 'y' if yes, 'n' if no: ");
            cscanc(&endchoice);
            if (endchoice == 'n' || endchoice == 'N')
                goto EndRec; // return 0;
        } while(!(endchoice == 'y' || endchoice == 'Y' || endchoice == 'n' || endchoice == 'N'));
    EndNoRec:
        return -1;
}
/**
 * @brief Teller Search/Update/Delete Request by Teller Name
 * 
 * @param teller_name Teller Name search
 * @param request "search" | "update" | "delete" #(all other string characters are ignored)
 * @return int 0 - done with search | else still searching
 */
int teller_search_name(const char * teller_name, const char * request) {
    clrscr();
    int i, k, l = 0, count, selectedIndex = -1, recordsCount = 0, selectedID = -1;
    char endchoice, first_name[26], middle_name[26], last_name[26];
    count = getRecordCount(TELLERRECORDS, sizeof(Teller));
    int selectedIndexes[count];
    memset(selectedIndexes, 0, sizeof(selectedIndexes));
    Teller tellers[count], tellerSelected[count], selectedTeller; // array of tellers with count
    memset(tellers, 0, sizeof(tellers));
    memset(tellerSelected, 0, sizeof(tellerSelected));
    memset(&selectedTeller, 0, sizeof(selectedTeller));
    if (count < 1)
        goto NoRecords; // no records found since count is 0
    getTellerData(tellers); // read from file to tellers struct
    printf("\n ---------- %s Teller Details ----------\n\n", capitalize(request));
    printf(" %s%s%s%s\n\n", "Teller ID", "     Teller First Name    ", "    Teller Middle Name    ", "     Teller Last Name     ");
    for (i=0; i < count; i++) {
        // search for first name
        for (k = 0; k < strlen(tellers[i].first_name); k++) {
            if (0 == strnicmp(tellers[i].first_name + k, teller_name, strlen(teller_name))) {
                // copy to selected
                tellerSelected[l].id = tellers[i].id;
                strcpy(tellerSelected[l].first_name, tellers[i].first_name);
                strcpy(tellerSelected[l].middle_name, tellers[i].middle_name);
                strcpy(tellerSelected[l].last_name, tellers[i].last_name);
                // copy index i to selectedIndexes[l]
                selectedIndexes[l] = i;
                // copy to display
                strcpy(first_name, centerTheString(tellers[i].first_name, sizeof(first_name)));
                strcpy(middle_name, centerTheString(tellers[i].middle_name, sizeof(middle_name)));

                strcpy(last_name, centerTheString(tellers[i].last_name, sizeof(last_name)));

                // display data
                printf("  %08d %s%s%s\n", tellers[i].id, first_name, middle_name, last_name);
                recordsCount++;
                l++; // l for tellerSelected index
                break; // break for k loop since name has been found
            }
        }
        // if not first name; then middle name
        for (k = 0; k < strlen(tellers[i].middle_name); k++) {
            if (0 == strnicmp(tellers[i].middle_name + k, teller_name, strlen(teller_name))) {
                if (l > 0)
                    if (tellerSelected[l-1].id == tellers[i].id)
                        break; // already selected so skip
                // copy to selected
                tellerSelected[l].id = tellers[i].id;
                strcpy(tellerSelected[l].first_name, tellers[i].first_name);
                strcpy(tellerSelected[l].middle_name, tellers[i].middle_name);
                strcpy(tellerSelected[l].last_name, tellers[i].last_name);
                // copy index i to selectedIndexes[l]
                selectedIndexes[l] = i;
                // copy to display
                strcpy(first_name, centerTheString(tellers[i].first_name, sizeof(first_name)));
                strcpy(middle_name, centerTheString(tellers[i].middle_name, sizeof(middle_name)));
                strcpy(last_name, centerTheString(tellers[i].last_name, sizeof(last_name)));
                // display data
                printf("  %08d %s%s%s\n", tellers[i].id, first_name, middle_name, last_name);
                recordsCount++;
                l++; // l for tellerSelected index
                break; // break for k loop since name has been found
            }
        }
        // if not first and middle name; then last name
        for (k = 0; k < strlen(tellers[i].last_name); k++) {
            if (0 == strnicmp(tellers[i].last_name + k, teller_name, strlen(teller_name))) {
                if (l > 0)
                    if (tellerSelected[l-1].id == tellers[i].id)
                        break; // already selected so skip
                // copy to selected
                tellerSelected[l].id = tellers[i].id;
                strcpy(tellerSelected[l].first_name, tellers[i].first_name);
                strcpy(tellerSelected[l].middle_name, tellers[i].middle_name);
                strcpy(tellerSelected[l].last_name, tellers[i].last_name);
                // copy index i to selectedIndexes[l]
                selectedIndexes[l] = i;
                // copy to display
                strcpy(first_name, centerTheString(tellers[i].first_name, sizeof(first_name)));
                strcpy(middle_name, centerTheString(tellers[i].middle_name, sizeof(middle_name)));
                strcpy(last_name, centerTheString(tellers[i].last_name, sizeof(last_name)));
                // display data
                printf("  %08d %s%s%s\n", tellers[i].id, first_name, middle_name, last_name);
                recordsCount++;
                l++; // l for tellerSelected index
                break; // break for k loop since name has been found
            }
        }
    }
    if (recordsCount > 0)
        goto Found;
    NotFound:
        goto EndNoRecTable; // no records found redirect
    Found:
        printf("\n --------------------------------------------\n\n");
        // Search / Update / Delete Functions
        if (0 == strcmp(request, "update")) {
            if (recordsCount > 1) {
                // make selection
                printf(" => Select one (1) row data to UPDATE:\n");
                do {
                    printf(" Enter ID: ");
                    customScanfDefaultInt(&selectedID, -1);
                    for (i = 0; i < l; i++) {
                        if (tellerSelected[i].id == selectedID) {
                            selectedIndex = selectedIndexes[i];
                            selectedTeller.id = tellerSelected[i].id;
                            strcpy(selectedTeller.first_name, tellerSelected[i].first_name);
                            strcpy(selectedTeller.middle_name, tellerSelected[i].middle_name);
                            strcpy(selectedTeller.last_name, tellerSelected[i].last_name);
                            goto IDSelectedUpdate;
                        }
                    }
                    selectedID = -1;
                    printf(" ID not in selection! Please Try Again.\n");
                } while(selectedID < 0);
            } else {
                selectedIndex = selectedIndexes[0];
                selectedTeller.id = tellerSelected[0].id;
                strcpy(selectedTeller.first_name, tellerSelected[0].first_name);
                strcpy(selectedTeller.middle_name, tellerSelected[0].middle_name);
                strcpy(selectedTeller.last_name, tellerSelected[0].last_name);
            }
            IDSelectedUpdate: // ID has selected label
                char savedata;
                Teller oldData;
                memset(&oldData, 0, sizeof(Teller));
                oldData.id = selectedTeller.id;
                strcpy(oldData.first_name, selectedTeller.first_name);
                strcpy(oldData.middle_name, selectedTeller.middle_name);
                strcpy(oldData.last_name, selectedTeller.last_name);
                fflush(stdin);
                // update selected data
                printf(" => Update Data:\n");
                printf(" Teller ID : %08d\n", oldData.id);
                printf(" Teller First Name (%s): ", oldData.first_name);
                customScanfDefaultString(selectedTeller.first_name, oldData.first_name);
                printf(" Teller Middle Name (%s): ", oldData.middle_name);
                customScanfDefaultString(selectedTeller.middle_name, oldData.middle_name);
                printf(" Teller Last Name (%s): ", oldData.last_name);
                customScanfDefaultString(selectedTeller.last_name, oldData.last_name);
                printf("\n Save modified data?\n");
                do {
                    printf(" Type 'y' if yes, 'n' if no: ");
                    cscanc(&savedata);
                    if (savedata == 'N' || savedata == 'n') {
                        goto SearchAgain; // cancelled / not saved
                    }
                } while (!(savedata == 'y' || savedata == 'Y' || savedata == 'n' || savedata == 'N'));
                // copy modified data to all tellers selected index
                tellers[selectedIndex].id = selectedTeller.id;
                strcpy(tellers[selectedIndex].first_name, selectedTeller.first_name);
                strcpy(tellers[selectedIndex].middle_name, selectedTeller.middle_name);
                strcpy(tellers[selectedIndex].last_name, selectedTeller.last_name);
                // save to file
                if (0 != saveTellerToFile(tellers, count)) {
                    printf(" Something went wrong. Try again.\n\n");
                    goto SearchAgain;
                }
                printf("\n ==> Successfully Updated Record to file!\n\n");
        } else if (0 == strcmp(request, "delete")) {
            // delete selected data
            if (recordsCount > 1) {
                // make selection
                printf(" => Select one (1) row data to DELETE:\n");
                do {
                    printf(" Enter ID: ");
                    customScanfDefaultInt(&selectedID, -1);
                    for (i = 0; i < l; i++) {
                        if (tellerSelected[i].id == selectedID)
                            goto IDSelectedDelete;
                    }
                    selectedID = -1;
                    printf(" ID not in selection! Please Try Again.\n");
                } while(selectedID < 0);
            } else
                selectedID = tellerSelected[0].id;
            IDSelectedDelete: // ID has selected label
                char dchoice;
                printf(" Selected ID: %08d\n", selectedID);
                printf(" => Do you really want to delete ID # %08d record?\n", selectedID);
                do {
                    printf(" Type 'y' if yes, 'n' if no: ");
                    cscanc(&dchoice);
                    if (dchoice == 'n' || dchoice == 'N')
                        goto SearchAgain;
                } while (!(dchoice == 'y' || dchoice == 'Y' || dchoice == 'n' || dchoice == 'N'));
                printf(" ==> Deleting Record...\n");
                int newCount = 0, j = 0;
                Teller tellersNew[count];
                memset(tellersNew, 0, sizeof(tellersNew));
                for (i = 0; i < count; i++) {
                    if (tellers[i].id == selectedID)
                        continue; // skip loop if selectedIndex is equal to i to be deleted
                    // else copy the record/row to tellersNew[i]
                    tellersNew[j].id = tellers[i].id;
                    strcpy(tellersNew[j].first_name, tellers[i].first_name);
                    strcpy(tellersNew[j].middle_name, tellers[i].middle_name);
                    strcpy(tellersNew[j].last_name, tellers[i].last_name);
                    j++; // j for tellersNew index
                    newCount++; // new count
                }
                if (0 != saveTellerToFile(tellersNew, newCount)) {
                    printf(" Something went wrong. Try again.\n\n");
                    goto SearchAgain;
                }
                printf(" ==> Successfully Deleted Record from file!\n\n");
        }
        goto SearchAgain;
    EndRec:
        return 0;
    NoRecords:
        printf("\n ---------- %s Teller Details ----------\n\n", capitalize(request));
    EndNoRecTable:
        printf(" => No Records found\n");
        printf("\n ---------------------------------------------\n\n");
    SearchAgain:
        do {
            printf(" Do you want to search again?\n");
            printf(" Type 'y' if yes, 'n' if no: ");
            cscanc(&endchoice);
            if (endchoice == 'n' || endchoice == 'N')
                goto EndRec; // return 0;
        } while(!(endchoice == 'y' || endchoice == 'Y' || endchoice == 'n' || endchoice == 'N'));
    EndNoRec:
        return -1;
}
/**
 * @brief Add new Sale Transaction
 * 
 */
void sale_new(void) {
    clrscr();
    fflush(stdin);
    char choice, i, appendDisplay[5000], buffile[MAX_NAME], datenow[TIME_SIZE], timenow[TIME_SIZE], tempbuf[MAX_NAME];
    int searchID, latestID, count = 0, index = 0, tempQuantity = -1;
    float payable_amount, cash = -1.0, change;
    time_t t;
    struct tm * tmp;
    time(&t); // set time
    tmp = localtime(&t); // set localtime
    memset(appendDisplay, 0, sizeof(appendDisplay)); // set appendDisplay to empty string
    memset(buffile, 0, sizeof(buffile)); // set to empty
    memset(datenow, 0, sizeof(datenow)); // set to empty
    memset(timenow, 0, sizeof(timenow)); // set to empty
    strftime(datenow, sizeof(datenow), "%Y-%m-%d", tmp); // format will be 2022-12-25 for the filename
    sprintf(buffile, SALETRANSACTIONS, datenow); // we will use date for the filename
    index = getRecordCount(SALERECORDS, sizeof(SaleTransaction));
    count = index;
    latestID = getLatestID(SALERECORDS, sizeof(SaleTransaction)); // set latest id of SaleTransaction data
    SaleTransaction sale[MAX_ITEMS];
    memset(sale, 0, sizeof(sale));
    if (count > 0)
        getSaleData(sale);
    strcat(appendDisplay, "\n ---------- New Transaction ----------\n");
    do {
        memset(&sale[count], 0, sizeof(sale[count])); // set SaleTransaction data to empty or 0
        memset(&sale[count].product, 0, sizeof(sale[count].product)); // set the Product data empty or 0
        sprintf(tempbuf, "\n Sale ID : %d\n%c", ++latestID, 0); // increment the latestID of recorded sale transaction
        sale[count].id = latestID; // put the latest ID to the SaleTransaction struct data
        strcat(appendDisplay, tempbuf);
        do {
            clrscr();
            printf("%s", appendDisplay);
            printf(" Product ID : "); // We will use product ID...
            customScanfDefaultInt(&searchID, -1); // ...rather than Product name for input to search the specific existing product
        } while (getProductByID(&sale[count].product, searchID) != 0); // searching for product details by ID and put it in the SaleTransaction data
        
        // append display to appendDisplay variable string of the selected product details     
        sprintf(tempbuf, " Product Name : %s\n%c", sale[count].product.name, 0);
        strcat(appendDisplay, tempbuf);
        sprintf(tempbuf, " Product Unit : %s\n%c", sale[count].product.unit, 0);
        strcat(appendDisplay, tempbuf);
        sprintf(tempbuf, " Product Price : %.2f\n%c", sale[count].product.unit_price, 0);
        strcat(appendDisplay, tempbuf);
        strcat(appendDisplay, " Quantity : ");
        do {
            clrscr();
            printf("%s", appendDisplay);
            customScanfDefaultInt(&tempQuantity, -1);
            if (tempQuantity < 1) {
                printf(" => Quantity should be greater (>) than 0.\n");
                getch();
            }
        } while (tempQuantity < 0);
        sale[count].quantity = tempQuantity;
        sprintf(tempbuf, "%d\n%c", sale[count].quantity, 0);
        strcat(appendDisplay, tempbuf);
        clrscr(); // clear the command line screen
        printf("%s", appendDisplay); // display previous and current product details of selected products
        count++; // increment the count of latest new records
        do {
            printf("\n Do you want to add another item?\n");
            printf(" Type 'y' if yes, 'n' if no: ");
            cscanc(&choice);
            if (!(choice == 'n' || choice == 'N' || choice == 'y' || choice == 'Y'))
                printf(" Invalid Choice!\n");
        } while (!(choice == 'n' || choice == 'N' || choice == 'y' || choice == 'Y'));
        // repeat if yes
    } while (!(choice == 'n' || choice == 'N'));
    clrscr(); // clear the command line screen
    payable_amount = compute_payable_amount(sale, count-index);
    // record payable amount
    strcat(appendDisplay, " _____________________________________\n");
    sprintf(tempbuf, " Total Payable Amount:\t%.2f\n%c", payable_amount, 0);
    strcat(appendDisplay, tempbuf);
    strcat(appendDisplay, " Cash: ");
    do {
        clrscr();
        // display total
        printf("%s", appendDisplay);
        // get cash amount
        customScanfDefaultFloat(&cash, -1.0);
        if (cash < payable_amount) {
            printf(" => Cash should be more than or equal to the total payable amount! Try again.\n");
            getch();
        }
    } while (cash < payable_amount);
    // compute change
    change = compute_change(payable_amount, cash);
    sprintf(tempbuf, "%.2f\n%c", cash, 0);
    strcat(appendDisplay, tempbuf);
    sprintf(tempbuf, "\n Change: %.2f%c", change, 0);
    strcat(appendDisplay, tempbuf);
    printf("\n Change: %.2f\n", change);
    time(&t); // set time
    tmp = localtime(&t); // set localtime
    strftime(timenow, sizeof(timenow), "%H:%M:%S", tmp); // format will be 24:59:59 for the time of transaction
    sprintf(tempbuf, "\n\n Date: %s\n%c", datenow, 0);
    strcat(appendDisplay, tempbuf);
    sprintf(tempbuf, " Time: %s\n%c", timenow, 0);
    strcat(appendDisplay, tempbuf);
    // write to bin file
    if (0 != saveSaleTransactionToFile(sale, count)) {
        fprintf(stderr, "Failed to write sales transaction records file. Sale Transaction was not saved");
        return;
    }
    // write to txt file
    FILE * fp;
    if ((fp = fopen(buffile, "a")) == NULL) {
        fprintf(stderr, "Failed to write transaction file. Sale Transaction saved but did not write to display transaction text file.");
        return;
    }
    fprintf(fp, "%s", appendDisplay);
    fclose(fp);
    getch();
}
/**
 * @brief Display all Sale Transactions
 * 
 */
void sale_display(void) {
    clrscr();
    int i, j, count, size;
    char name[20], p_unit[20], p_price[16];
    count = getRecordCount(SALERECORDS, sizeof(SaleTransaction));
    SaleTransaction sales[count];
    if (count < 1)
        goto displayEmptyResults; // redirect empty records
    getSaleData(sales);
    printf("\n ---------- Display Transaction ----------\n\n");
    printf(" %s%s%s%s%s\n\n", "  Sale ID ", "    Product Name    ", "    Product Unit    ", " Product Unit Price ", " Quantity ");
    for (i = 0; i < count; i++) {
        strcpy(name, centerTheString(sales[i].product.name, sizeof(name)));
        strcpy(p_unit, centerTheString(sales[i].product.unit, sizeof(p_unit)));
        // display data
        printf("  %08d %s%s", sales[i].id, name, p_unit);
        strcpy(p_price, rightAlignFloat(sales[i].product.unit_price, sizeof(p_price)));
        printf("%s  \t   %d\n", p_price, sales[i].quantity);
    }
    printf("\n -----------------------------------------\n");
    getch();
    return;
    displayEmptyResults:
        printf("\n ---------- Display Transaction ----------\n\n");
        printf(" %s%s%s%s%s\n\n", "  Sale ID ", "    Product Name    ", "    Product Unit    ", " Product Unit Price ", " Quantity ");
        printf("\n -----------------------------------------\n");
        getch();
}
/**
 * @brief Compute payable amount of sales and return the amount
 * 
 * @param sale SaleTransaction structure data
 * @return float Total Payable Amount
 */
float compute_payable_amount(SaleTransaction * sale, int count) {
    int i, starti;
    float sum;
    starti = getRecordCount(SALERECORDS, sizeof(SaleTransaction));
    for (i = starti; i < count+starti; i++)
        sum += sale[i].product.unit_price * sale[i].quantity;
    return sum;
}
/**
 * @brief Compute the change of total payable amount and cash given and return the change amount
 * 
 * @param sale SaleTransaction structure data
 * @return float Total Change Amount
 */
float compute_change(float payable_amount, float cash) {
    return (float)(cash - payable_amount);
}
/**
 * @brief Get the Record Count from file
 * 
 * @param filename 
 * @param recordsize size of each record/row
 * @return int count of rows
 */
int getRecordCount(const char * filename, int recordsize) {
    FILE * fp;
    int size, count = 0;
    if ((fp = fopen(filename, "rb")) == NULL) {
        fprintf(stderr, "Failed to open %s Records.", filename);
        return 0;
    }
    fseek(fp, 0, SEEK_END); // set cursor at the end
    size = ftell(fp); // get the file size from the end cursor
    fseek(fp, 0, SEEK_SET);
    fclose(fp);
    if (!size)
        return 0; // if file size is 0 return 0
    count = size / recordsize; // get the records count by dividing the file size and the size of Teller struct
    return count;
}
/**
 * @brief Get the Latest ID from records
 * 
 * @param filename 
 * @param recordsize size of each record/row
 * @return int latest maximum ID; 0 if file error
 */
int getLatestID(const char * filename, int recordsize) {
    int i, count, maxid;
    count = getRecordCount(filename, recordsize);
    if (count < 1)
        return 0; // return 0 if no records
    int ids[count];
    if (0 == strcmp(filename, PRODUCTRECORDS)) {
        Product temp[count];
        memset(temp, 0, sizeof(temp));
        getProductData(temp);
        for (i = 0; i < count; i++) {
            ids[i] = temp[i].id;
        }
    } else if (0 == strcmp(filename, TELLERRECORDS)) {
        Teller temp[count];
        memset(temp, 0, sizeof(temp));
        getTellerData(temp);
        for (i = 0; i < count; i++) {
            ids[i] = temp[i].id;
        }
    } else if (0 == strcmp(filename, SALERECORDS)) {
        SaleTransaction temp[count];
        memset(temp, 0, sizeof(temp));
        getSaleData(temp);
        for (i = 0; i < count; i++) {
            ids[i] = temp[i].id;
        }
    }
    maxid = maxOfInt(ids, count);
    return maxid;
}
/**
 * @brief Save Product struct to file
 * 
 * @param product Product struct data
 * @param count count of all new data
 * @return int 0 - success | -1 error
 */
int saveProductToFile(Product * product, int count) {
    int i;
    FILE * fp;
    if ((fp = fopen(PRODUCTRECORDS, "wb")) == NULL) {
        fprintf(stderr, "CANNOT READ PRODUCT RECORDS FILE.\n");
        return -1;
    }
    for (i = 0; i < count; i++)
        fwrite(&product[i], sizeof(Product), 1, fp);
    fclose(fp);
    return 0;
}
/**
 * @brief Save Teller struct to file
 * 
 * @param teller Teller struct data
 * @param count count of all new data
 * @return int 0 - success | -1 error
 */
int saveTellerToFile(Teller * teller, int count) {
    int i;
    FILE * fp;
    if ((fp = fopen(TELLERRECORDS, "wb")) == NULL) {
        fprintf(stderr, "CANNOT READ TELLER RECORDS FILE.\n");
        return -1;
    }
    for (i = 0; i < count; i++)
        fwrite(&teller[i], sizeof(Teller), 1, fp);
    fclose(fp);
    return 0;
}
/**
 * @brief Save SaleTransaction struct to file
 * 
 * @param teller SaleTransaction struct data
 * @param count count of all new data
 * @return int 0 - success | -1 error
 */
int saveSaleTransactionToFile(SaleTransaction * sale, int count) {
    int i;
    FILE * fp;
    if ((fp = fopen(SALERECORDS, "wb")) == NULL) {
        fprintf(stderr, "CANNOT READ SALE RECORDS FILE.\n");
        return -1;
    }
    for (i = 0; i < count; i++)
        fwrite(&sale[i], sizeof(SaleTransaction), 1, fp);
    fclose(fp);
    return 0;
} 
/**
 * @brief Get the Product Data from records file
 * 
 * @param product Product buffer
 */
void getProductData(Product * product) {
    FILE * fp;
    if ((fp = fopen(PRODUCTRECORDS, "rb")) == NULL) {
        fprintf(stderr, "CANNOT READ PRODUCT RECORDS FILE.\n");
        return;
    }
    int count, i;
    count = getRecordCount(PRODUCTRECORDS, sizeof(Product));
    if (count < 1)
        return;
    for (i = 0; i < count; i++)
        fread(&product[i], sizeof(Product), 1, fp);
    fclose(fp);
}
/**
 * @brief Get the Teller Data from file
 * 
 * @param teller Teller buffer
 */
void getTellerData(Teller * teller) {
    FILE * fp;
    if ((fp = fopen(TELLERRECORDS, "rb")) == NULL) {
        fprintf(stderr, "CANNOT READ TELLER RECORDS FILE.\n");
        return;
    }
    int count, i;
    count = getRecordCount(TELLERRECORDS, sizeof(Teller));
    if (count < 1)
        return;
    for (i = 0; i < count; i++)
        fread(&teller[i], sizeof(Teller), 1, fp);
    fclose(fp);
}
/**
 * @brief Get the Sale Data from file
 * 
 * @param sale SaleTransaction buffer
 */
void getSaleData(SaleTransaction * sale) {
    FILE * fp;
    if ((fp = fopen(SALERECORDS, "rb")) == NULL) {
        fprintf(stderr, "CANNOT READ SALE RECORDS FILE.\n");
        return;
    }
    int count, i;
    count = getRecordCount(SALERECORDS, sizeof(SaleTransaction));
    if (count < 1)
        return;
    for (i = 0; i < count; i++)
        fread(&sale[i], sizeof(SaleTransaction), 1, fp);
    fclose(fp);
}
/**
 * @brief Get the Product struct By ID
 * 
 * @param productbuffer Product struct buffer
 * @param searchID Product ID search
 * @return int 0 - success | -1 not found
 */
int getProductByID(Product * productbuffer, int searchID) {
    int count, i;
    count = getRecordCount(PRODUCTRECORDS, sizeof(Product));
    Product products[count];
    getProductData(products);
    for (i = 0; i < count; i++) {
        if (products[i].id == searchID) {
            (*productbuffer).id = products[i].id;
            strcpy((*productbuffer).name, products[i].name);
            strcpy((*productbuffer).description, products[i].description);
            strcpy((*productbuffer).category, products[i].category);
            strcpy((*productbuffer).unit, products[i].unit);
            (*productbuffer).unit_price = products[i].unit_price;
            return 0; // found
        }
    }
    printf(" => Product not found! Try again.\n");
    getch();
    return -1; // not found
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
    memset(buf, 0, 2);
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
/**
 * @brief Get the maximum value of an int array
 * 
 * @param arr array of integers
 * @return int maximum value
 */
int maxOfInt(int * arr, int count) {
    int i;
    if (count < 1)
        return 0;
    for (i = 0; i < count; i++) {
        if (arr[0] < arr[i]) {
            arr[0] = arr[i];
        }
    }
    return arr[0];
}
/**
 * @brief Center the string content by size of char
 * 
 * @param Str content
 * @param size size of char
 * @return char* centered string with size of param size
 */
char * centerTheString(const char * strval, int size) {
    char buf[size], *ret;
    int i = 0, j=0, lenOfStr, divideTwo = 2, minusOne = 1, end, spaces, lspace, rspace;
    lenOfStr = strlen(strval);
    memset(buf, 0, sizeof(buf)); // empty the string buffer
    if (lenOfStr>size)
        lenOfStr = size;
    spaces = size - lenOfStr;
    if (spaces % 2 == 1) {
        lspace = (spaces + minusOne) / divideTwo;
    } else {
        lspace = spaces / divideTwo;
    }
    end = lenOfStr + lspace; // after content
    rspace = size - (lenOfStr + lspace); // size of right spaces
    memset(buf, ' ', lspace); // set left spaces
    strncpy(buf + lspace, strval, lenOfStr); // set content in the middle
    memset(buf + end, ' ', rspace); // set right spaces
    buf[size] = 0; // set the last character to endline/null character
    ret = (char *)buf; // return the char pointer with (char *)buf value
    return ret;
}
/**
 * @brief Right Align Float Value in a String Format
 * 
 * @param value floating value
 * @param charsize size of char
 * @return char* String Formatted value with right-aligned position in accordance with char size
 */
char * rightAlignFloat(float value, int size) {
    char buffer[size], *ret, temp[size];
    int lenOfValue = 0, minusOne = 1, spaces = 0;
    memset(buffer, 0, sizeof(buffer));
    memset(temp, 0, size);
    sprintf(temp, "%.2f", value);
    lenOfValue = strlen(temp);
    spaces = size - lenOfValue;
    memset(buffer, ' ', size);
    strncpy(buffer + spaces, temp, lenOfValue); // copy the string value of float at the end after spaces
    buffer[size] = 0; // set the last character to endline/null character
    ret = (char *)buffer;
    return ret;
}
/**
 * @brief Custom scanf with default string if input is empty
 * 
 * @param buffer char[] (string) buffer
 * @param defaultVal default string value
 */
void customScanfDefaultString(char * buffer, const char * defaultVal) {
    char buf[MAX_NAME], defaultStr[MAX_NAME];
    memset(defaultStr, 0, sizeof(defaultStr));
    memset(buf, 0, sizeof(buf));
    strcpy(defaultStr, defaultVal);
    scanf("%[^\n]s", buf);
    fflush(stdin);
    if (strlen(buf) < 1)
        strcpy(buffer,defaultStr);
    else {
        strcpy(buffer, buf);
    }
}
/**
 * @brief Custom Scanf with default floating number if input is empty
 * 
 * @param buffer floating number buffer
 * @param defaultVal default float value
 */
void customScanfDefaultFloat(float * buffer, float defaultVal) {
    int i, dots = 0;
    float result;
    char buf[MAX_NAME];
    memset(buf, 0, sizeof(buf));
    scanf("%[^\n]s", buf);
    fflush(stdin);
    if (strlen(buf) < 1)
        goto DefaultValue;
    for (i=0; i < strlen(buf); i++) {
        if (!(isdigit(buf[i]) || buf[i] == '.')) {
            printf(" Invalid Input. Only Numeric characters are allowed!\n");
            goto DefaultValue;
        }
        if (buf[i] == '.') dots++;
        if (dots > 1) {
            printf(" Invalid Input. Cannot input more than one dot character (.) in a numeric input\n");
            goto DefaultValue;
        }
    }
    // if all digits or . (dot)
    if (strlen(buf) == 1 && buf[0] == '.')
        *buffer = 0.0;
    else {
        result = (float)atof(buf);
        *buffer = result;
    }
    return;
    DefaultValue:
        *buffer = defaultVal;
}
/**
 * @brief Custom Scanf with default integer number if input is empty
 * 
 * @param buffer integer buffer
 * @param defaultVal default int value
 */
void customScanfDefaultInt(int * buffer, const int defaultVal) {
    int i, dots = 0;
    int result;
    char buf[MAX_NAME];
    scanf("%[^\n]s", buf);
    fflush(stdin);
    if (strlen(buf) < 1)
        goto DefaultValue;
    for (i=0; i < strlen(buf); i++) {
        if (!(isdigit(buf[i]) || buf[i] == '.')) {
            printf(" Invalid Input. Only Numeric characters are allowed!\n");
            goto DefaultValue;
        }
        if (buf[i] == '.') dots++;
        if (dots > 1) {
            printf(" Invalid Input. Cannot input more than one dot character (.) in a numeric input\n");
            goto DefaultValue;
        }
    }
    // if all digits or . (dot)
    if (strlen(buf) == 1 && buf[0] == '.')
        *buffer = 0;
    else {
        result = (int)atoi(buf);
        *buffer = result;
    }
    return;
    DefaultValue:
        *buffer = defaultVal;
}