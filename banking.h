#ifndef BANKING_H
#define BANKING_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "uthash.h"

#define MAX_USERNAME_LENGTH 20
#define MAX_PASSWORD_LENGTH 41
#define MAX_ACCOUNT_ID_LENGTH 30
#define MAX_ACCOUNT_ID_LENGHT 30
#define MAX_OPERATION_CODE 20

#define LEFTROTATE(x, c) (((x) << (c)) | ((x) >> (32 - (c))))

typedef struct user {
    int id;
    char username[MAX_USERNAME_LENGTH];
    char hashed_password[MAX_PASSWORD_LENGTH];
    UT_hash_handle hh;
} user;

typedef struct account {
    char account_id[MAX_ACCOUNT_ID_LENGTH];
    int user_id;
    float balance;
    UT_hash_handle hh;
} account;

typedef struct transaction {
    char operation_code[MAX_OPERATION_CODE];
    float amount;
    char from_account[MAX_ACCOUNT_ID_LENGHT];
    char to_account[MAX_ACCOUNT_ID_LENGHT];
    int processed; 
    struct transaction *next;
} transaction;

void ensure_data_directory();
void handle_program_exit(user **users, account **accounts, transaction **transactions);

void sha1(const uint8_t *data, size_t len, uint8_t hash_out[20]);
char *hash_password(const char *password);
int compare_password(char *password, char *hashed_password);
user *register_user(user **users, account **accounts);
user *login_user(user **users);
void logout(user **user);

void load_users(user **users);
void save_users(user **users);
void append_user_to_file(user *new_user);
int generate_unique_user_id(user **users);

void load_accounts(account **accounts);
void save_accounts(account **accounts);
account* find_user_account(account **accounts, user *current_user);
void create_account_for_user(account **accounts, int user_id);
void check_balance(account **accounts, user *current_user);
void deposit(account **accounts, user *current_user);
void withdraw(account **accounts, user *current_user);

transaction* load_transactions_from_file();
void append_transaction_to_file(const char *operation_code, float amount, const char *from_account, const char *to_account, int processed);
transaction* add_transaction(transaction *head, const char *operation_code, float amount, const char *from_account, const char *to_account);
void update_transactions_file(transaction *head);
transaction* process_transactions(transaction *head, account **accounts);
void transfer(account **accounts, user *current_user, transaction **transactions);

int auth_menu();
int main_menu();

#endif 