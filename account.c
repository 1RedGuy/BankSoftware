#include "banking.h"

void load_accounts(account **accounts) {
    FILE *file = fopen("data/accounts.txt", "r");
    if (file == NULL) {
        printf("Accounts file not found, starting fresh.\n");
        return;
    }

    char line[100];
    while (fgets(line, sizeof(line), file)) {
        account *new_account = malloc(sizeof(account));
        if (!new_account) {
            printf("Memory allocation failed for account.\n");
            continue;
        }
        sscanf(line, "%s %d %f", new_account->account_id, &new_account->user_id, &new_account->balance);
        HASH_ADD_STR(*accounts, account_id, new_account);
    }

    fclose(file);
}

void save_accounts(account **accounts) {
    FILE *file = fopen("data/accounts.txt", "w");
    if (!file) {
        printf("Error opening accounts.txt for writing!\n");
        return;
    }

    account *tmp, *tmp2;
    HASH_ITER(hh, *accounts, tmp, tmp2) {
        fprintf(file, "%s %d %f\n", tmp->account_id, tmp->user_id, tmp->balance);
    }
    fclose(file);
    printf("Accounts saved successfully.\n");
}

account* find_user_account(account **accounts, user *current_user) {
    account *acc, *tmp;
    HASH_ITER(hh, *accounts, acc, tmp) {
        if (acc->user_id == current_user->id) {
            return acc;
        }
    }
    return NULL;
}

void create_account_for_user(account **accounts, int user_id) {
    account *new_account = malloc(sizeof(account));
    if (!new_account) {
        printf("Failed to allocate memory for new account.\n");
        return;
    }

    sprintf(new_account->account_id, "ACC%03d", user_id);
    new_account->user_id = user_id;
    new_account->balance = 0.0f;

    HASH_ADD_STR(*accounts, account_id, new_account);

    save_accounts(accounts);

    printf("Account created for user ID %d with account ID %s\n", user_id, new_account->account_id);
}

void check_balance(account **accounts, user *current_user) {
    account *acc = find_user_account(accounts, current_user);

    if (acc == NULL) {
        printf("No account found for your user.\n");
        return;
    }

    printf("Account ID: %s\n", acc->account_id);
    printf("Current Balance: %.2f\n", acc->balance);
}

void deposit(account **accounts, user *current_user) {
    account *acc = find_user_account(accounts, current_user);

    if (acc == NULL) {
        printf("No account found for your user.\n");
        return;
    }

    float amount;
    printf("Enter amount to deposit: ");
    if (scanf("%f", &amount) != 1 || amount <= 0) {
        printf("Invalid amount entered. Deposit canceled.\n");
        return; 
    }

    acc->balance += amount;
    printf("Deposit successful. New balance: %.2f\n", acc->balance);
    save_accounts(accounts);
}

void withdraw(account **accounts, user *current_user) {
    account *acc = find_user_account(accounts, current_user);

    if (acc == NULL) {
        printf("No account found for your user.\n");
        return;
    }

    float amount;
    printf("Enter amount to withdraw: ");
    scanf("%f", &amount);
    if (amount <= 0) {
        printf("Invalid amount entered.\n");
        return;
    }

    if (amount > acc->balance) {
        printf("Insufficient balance! Current balance: %.2f\n", acc->balance);
        return;
    }

    acc->balance -= amount;
    printf("Withdrawal successful. New balance: %.2f\n", acc->balance);
    save_accounts(accounts);
} 