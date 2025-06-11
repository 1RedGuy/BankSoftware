#include "banking.h"

void ensure_data_directory() {
    struct stat st = {0};
    if (stat("data", &st) == -1) {
        if (mkdir("data", 0700) == 0) {
            printf("Created data directory.\n");
        } else {
            printf("Error creating data directory.\n");
        }
    }
}

void handle_program_exit(user **users, account **accounts, transaction **transactions) {
    printf("Saving data and exiting program...\n");

    save_users(users);
    save_accounts(accounts);

    user *curr_user, *tmp_user;
    HASH_ITER(hh, *users, curr_user, tmp_user) {
        HASH_DEL(*users, curr_user);
        free(curr_user);
    }

    account *curr_acc, *tmp_acc;
    HASH_ITER(hh, *accounts, curr_acc, tmp_acc) {
        HASH_DEL(*accounts, curr_acc);
        free(curr_acc);
    }

    transaction *curr_tx = *transactions;
    while (curr_tx) {
        transaction *tmp_tx = curr_tx;
        curr_tx = curr_tx->next;
        free(tmp_tx);
    }

    exit(0);
} 