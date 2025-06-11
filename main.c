#include "banking.h"


int main() {
    ensure_data_directory();

    FILE *f = fopen("data/users.txt", "a+");
    if (f) fclose(f);
    f = fopen("data/accounts.txt", "a+");
    if (f) fclose(f);

    user *users = NULL;
    load_users(&users);

    account *accounts = NULL;
    load_accounts(&accounts);

    transaction *transactions = load_transactions_from_file();
    transactions = process_transactions(transactions, &accounts);

    printf("Welcome to your Banking System\n");

    while (1) {
        int decision = auth_menu();
        user *user = NULL;

        switch (decision) {
            case 1: user = login_user(&users); break;
            case 2: user = register_user(&users, &accounts);; break;
            case 3: handle_program_exit(&users, &accounts, &transactions); break;
        }

        while (user != NULL) {
            int decision = main_menu();
            switch (decision) {
                case 1:
                    check_balance(&accounts, user);
                    break;
                case 2:
                    deposit(&accounts, user);
                    break;
                case 3:
                    withdraw(&accounts, user);
                    break;
                case 4:
                    transfer(&accounts, user, &transactions);
                    break;
                case 5:
                    logout(&user);
                    break;
            }
        }
    }

    return 0;
}
