#include "banking.h"

int auth_menu() {
    int decision;

    printf("\nChoose one of the following options:\n");
    printf("1. Login\n");
    printf("2. Register\n");
    printf("3. Exit\n");

    scanf("%d", &decision);
    while(getchar() != '\n');

    while (decision < 1 || decision > 3) {
        printf("Please try again!\n");
        scanf("%d", &decision);
    }

    return decision;
}

int main_menu() {
    int choice;
    printf("\n--- Main Menu ---\n");
    printf("1. Check Balance\n");
    printf("2. Deposit\n");
    printf("3. Withdraw\n");
    printf("4. Transfer\n");
    printf("5. Logout\n");
    printf("Select an option: ");
    scanf("%d", &choice);
    while(getchar() != '\n');
    while (choice < 1 || choice > 5) {
        printf("Invalid input. Try again: ");
        scanf("%d", &choice);
    }
    return choice;
} 