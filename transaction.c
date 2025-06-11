#include "banking.h"

void append_transaction_to_file(const char *operation_code, float amount, const char *from_account, const char *to_account, int processed) {
    FILE *file = fopen("data/transactions.txt", "a");
    if (!file) {
        printf("Error opening transactions.txt for writing.\n");
        return;
    }
    fprintf(file, "%s %.2f %s %s %d\n", operation_code, amount, from_account, to_account, processed);
    fclose(file);
}

transaction* add_transaction(transaction *head, const char *operation_code, float amount, const char *from_account, const char *to_account) {
    transaction *new_tx = malloc(sizeof(transaction));
    if (!new_tx) {
        printf("Memory allocation failed!\n");
        return head;
    }

    strcpy(new_tx->operation_code, operation_code);
    new_tx->amount = amount;
    strcpy(new_tx->from_account, from_account);
    strcpy(new_tx->to_account, to_account);
    new_tx->processed = 0; 
    new_tx->next = NULL;

    if (!head) {
        head = new_tx;
    } else {
        transaction *curr = head;
        while (curr->next) curr = curr->next;
        curr->next = new_tx;
    }

    append_transaction_to_file(operation_code, amount, from_account, to_account, 0);
    printf("Transaction added.\n");
    return head;
}

transaction* load_transactions_from_file() {
    FILE *file = fopen("data/transactions.txt", "r");
    if (!file) {
        printf("No transaction file found.\n");
        return NULL;
    }

    transaction *head = NULL;
    char op_code[MAX_OPERATION_CODE];
    float amount;
    char from_acc[MAX_ACCOUNT_ID_LENGHT];
    char to_acc[MAX_ACCOUNT_ID_LENGHT];
    int processed;

    while (fscanf(file, "%s %f %s %s %d", op_code, &amount, from_acc, to_acc, &processed) == 5) {
        transaction *new_tx = malloc(sizeof(transaction));
        if (!new_tx) {
            printf("Memory allocation failed for transaction.\n");
            continue;
        }

        strcpy(new_tx->operation_code, op_code);
        new_tx->amount = amount;
        strcpy(new_tx->from_account, from_acc);
        strcpy(new_tx->to_account, to_acc);
        new_tx->processed = processed;
        new_tx->next = NULL;

        if (!head) {
            head = new_tx;
        } else {
            transaction *curr = head;
            while (curr->next) curr = curr->next;
            curr->next = new_tx;
        }
    }

    fclose(file);
    return head;
}

void update_transactions_file(transaction *head) {
    FILE *file = fopen("data/transactions.txt", "w");
    if (!file) {
        printf("Error opening transactions.txt for writing.\n");
        return;
    }

    transaction *tx = head;
    while (tx) {
        fprintf(file, "%s %.2f %s %s %d\n", tx->operation_code, tx->amount, 
                tx->from_account, tx->to_account, tx->processed);
        tx = tx->next;
    }

    fclose(file);
}

transaction* process_transactions(transaction *head, account **accounts) {
    transaction *tx = head;
    int processed_any = 0;

    while (tx) {
        if (tx->processed == 0) {
            account *from = NULL, *to = NULL;
            HASH_FIND_STR(*accounts, tx->from_account, from);
            HASH_FIND_STR(*accounts, tx->to_account, to);

            if (from && to && from->balance >= tx->amount) {
                from->balance -= tx->amount;
                to->balance += tx->amount;
                tx->processed = 1; 
                printf("Processed: %.2f from %s to %s\n", tx->amount, tx->from_account, tx->to_account);
                processed_any = 1;
            } else {
                printf("Failed: %.2f from %s to %s (insufficient funds or invalid accounts)\n", 
                       tx->amount, tx->from_account, tx->to_account);
            }
        }

        tx = tx->next;
    }

    if (processed_any) {
        save_accounts(accounts);
        update_transactions_file(head);
    }

    return head; 
}

void transfer(account **accounts, user *current_user, transaction **transactions) {
    char to_account_id[MAX_ACCOUNT_ID_LENGHT];
    float amount;

    account *from_acc = find_user_account(accounts, current_user);

    if (!from_acc) {
        printf("No account found for your user.\n");
        return;
    }

    printf("Enter destination account ID: ");
    scanf("%s", to_account_id);

    account *to_acc = NULL;
    HASH_FIND_STR(*accounts, to_account_id, to_acc);

    if (!to_acc) {
        printf("Destination account does not exist.\n");
        return;
    }

    printf("Enter amount to transfer: ");
    if (scanf("%f", &amount) != 1 || amount <= 0) {
        printf("Invalid amount entered.\n");
        return;
    }

    if (from_acc->balance < amount) {
        printf("Insufficient funds.\n");
        return;
    }

    from_acc->balance -= amount;
    to_acc->balance += amount;

    *transactions = add_transaction(*transactions, "TRANSFER", amount, from_acc->account_id, to_account_id);

    printf("Transfer successful. New balance: %.2f\n", from_acc->balance);

    save_accounts(accounts);
} 