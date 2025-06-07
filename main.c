#include<stdio.h>
#include "uthash.h"
#include "sha1.h"

#define MAX_USERNAME_LENGHT 20
#define MAX_PASSWORD_LENGHT 41
#define MAX_ACCOUNT_ID_LENGHT 30

#define LEFTROTATE(x, c) (((x) << (c)) | ((x) >> (32 - (c))))

typedef struct user{
    int id;
    char username[MAX_USERNAME_LENGHT];
    char hashed_password[MAX_PASSWORD_LENGHT];
    UT_hash_handle hh; 
} user; 

typedef struct account{
    char account_id[MAX_ACCOUNT_ID_LENGHT];
    int user_id; 
    float balance;
    UT_hash_handle hh; 
} account; 

void load_users(user **users){
    FILE * file = fopen("users.txt", "r"); 
    if(file == NULL){
        printf("Error opening file! \n"); 
        return; 
    }

    char line[100];

    while(fgets(line, sizeof(line), file) != NULL){
        user *new_user = (struct user*)malloc(sizeof(user)); 
        sscanf(line, "%d %s %s", &new_user->id, new_user->username, new_user->hashed_password); 
        HASH_ADD_INT(*users, id, new_user); 
    }

    fclose(file);
}

void load_accounts(account **accounts){
    FILE * file = fopen("accounts.txt", "r"); 
    if(file == NULL){
        printf("Error opening file! \n"); 
        return; 
    }

    char line[100]; 

    while(fgets(line, sizeof(line), file) != NULL){
        account *new_account = (struct account*)malloc(sizeof(account));
        sscanf(line, "%s %d %f", new_account->account_id, &new_account->user_id, &new_account->balance); 
        HASH_ADD_STR(*accounts, account_id, new_account); 
    }

    fclose(file); 
}

int generate_unique_user_id(user **users){
    int id = 1; 
    user *tmp = NULL; 
    HASH_ITER(hh, *users, tmp, tmp){
        if(tmp->id >= id){
            id = tmp->id + 1; 
        }
    }
    return id; 
}

int compare_password(char *password, char *hashed_password){
    char *hashed = hash_password(password);
    int result = strcmp(hashed, hashed_password);
    free(hashed); 
    return result;  
}

void sha1(const uint8_t *data, size_t len, uint8_t hash_out[20]) {
    uint32_t h0 = 0x67452301;
    uint32_t h1 = 0xEFCDAB89;
    uint32_t h2 = 0x98BADCFE;
    uint32_t h3 = 0x10325476;
    uint32_t h4 = 0xC3D2E1F0;

    size_t new_len = len + 1;
    while (new_len % 64 != 56) new_len++;

    uint8_t *msg = calloc(new_len + 8, 1);
    memcpy(msg, data, len);
    msg[len] = 0x80;

    uint64_t bits_len = 8 * len;
    for (int i = 0; i < 8; i++) {
        msg[new_len + i] = (bits_len >> ((7 - i) * 8)) & 0xFF;
    }

    for (size_t offset = 0; offset < new_len + 8; offset += 64) {
        uint32_t w[80];
        for (int i = 0; i < 16; i++) {
            w[i] = (msg[offset + i*4] << 24) | (msg[offset + i*4 + 1] << 16)
                 | (msg[offset + i*4 + 2] << 8) | (msg[offset + i*4 + 3]);
        }
        for (int i = 16; i < 80; i++) {
            w[i] = LEFTROTATE(w[i-3] ^ w[i-8] ^ w[i-14] ^ w[i-16], 1);
        }

        uint32_t a = h0;
        uint32_t b = h1;
        uint32_t c = h2;
        uint32_t d = h3;
        uint32_t e = h4;

        for (int i = 0; i < 80; i++) {
            uint32_t f, k;
            if (i < 20) {
                f = (b & c) | ((~b) & d);
                k = 0x5A827999;
            } else if (i < 40) {
                f = b ^ c ^ d;
                k = 0x6ED9EBA1;
            } else if (i < 60) {
                f = (b & c) | (b & d) | (c & d);
                k = 0x8F1BBCDC;
            } else {
                f = b ^ c ^ d;
                k = 0xCA62C1D6;
            }

            uint32_t temp = LEFTROTATE(a, 5) + f + e + k + w[i];
            e = d;
            d = c;
            c = LEFTROTATE(b, 30);
            b = a;
            a = temp;
        }

        h0 += a; h1 += b; h2 += c; h3 += d; h4 += e;
    }

    free(msg);

    hash_out[0] = h0 >> 24; hash_out[1] = h0 >> 16; hash_out[2] = h0 >> 8; hash_out[3] = h0;
    hash_out[4] = h1 >> 24; hash_out[5] = h1 >> 16; hash_out[6] = h1 >> 8; hash_out[7] = h1;
    hash_out[8] = h2 >> 24; hash_out[9] = h2 >> 16; hash_out[10] = h2 >> 8; hash_out[11] = h2;
    hash_out[12] = h3 >> 24; hash_out[13] = h3 >> 16; hash_out[14] = h3 >> 8; hash_out[15] = h3;
    hash_out[16] = h4 >> 24; hash_out[17] = h4 >> 16; hash_out[18] = h4 >> 8; hash_out[19] = h4;
}

char *hash_password(const char *password) {
    uint8_t hash[20];
    sha1((const uint8_t *)password, strlen(password), hash);

    char *hex = malloc(41); // 20 байта * 2 + null terminator
    for (int i = 0; i < 20; i++) {
        sprintf(hex + i*2, "%02x", hash[i]);
    }
    hex[40] = '\0';
    return hex;
}


void save_users(user **users){
    FILE * file = fopen("users.txt", "w"); 
    if(file == NULL){
        printf("Error opening file! \n"); 
        return; 
    }

    user *tmp = NULL; 
    HASH_ITER(hh, *users, tmp, tmp){
        fprintf(file, "%d %s %s\n", tmp->id, tmp->username, tmp->hashed_password); 
    }
    fclose(file); 
}

void save_accounts(account **accounts){
    FILE * file = fopen("accounts.txt", "w"); 
    if(file == NULL){
        printf("Error opening file! \n"); 
        return; 
    }

    account *tmp = NULL; 
    HASH_ITER(hh, *accounts, tmp, tmp){
        fprintf(file, "%s %d %f\n", tmp->account_id, tmp->user_id, tmp->balance); 
    }
    fclose(file); 
}

user *register_user(user **users){
    char username[MAX_USERNAME_LENGHT];
    char password[MAX_PASSWORD_LENGHT]; 

    int username_exists = 1; 
    user *tmp = NULL; 
    do{
        printf("Enter your username: "); 
        scanf("%s", username); 

        HASH_ITER(hh, *users, tmp, tmp){
            if(strcmp(tmp->username, username) == 0){
                printf("Username already exists! \n"); 
                username_exists = 1;
                break; 
            }
            else{
                username_exists = 0; 
            }
        }
    }while(username_exists); 

    printf("Enter your password: "); 
    scanf("%s", password);


    user *new_user = (struct user*)malloc(sizeof(user));
    new_user->id = generate_unique_user_id(users); 
    strcpy(new_user->username, username); 
    strcpy(new_user->hashed_password, hash_password(password)); 

    HASH_ADD_INT(*users, id, new_user); 

    save_users(users); 

    printf("User registered successfully! \n"); 
    return new_user; 
}


user *login_user(user **users){
    char username[MAX_USERNAME_LENGHT];
    char password[MAX_PASSWORD_LENGHT]; 
    printf("Enter your username: "); 
    scanf("%s", username); 
    printf("Enter your password: "); 
    scanf("%s", password); 

    user *user = NULL, *tmp = NULL; 
    
    HASH_ITER(hh, *users, user, tmp){
        if(strcmp(user->username, username) == 0){
            if(compare_password(password, user->hashed_password) == 0){
                printf("Login successful! \n"); 
                return user; 
            }
        }
    }
    printf("Invalid username or password! \n"); 
    return NULL; 
}

void logout(user **user){
    *user = NULL; 
}

int auth_menu(){

    int decision; 

    printf("Choose one of the following options: \n");
    printf("Login - 1 \n");
    printf("Register - 2 \n");
    printf("Exit - 3 \n");

    scanf("%d", &decision);

    while(decision != 1 && decision != 2 && decision != 3){
        printf("Please try again! \n"); 
        scanf("%d", &decision);
    }

    return decision; 
}

int main_menu(){
    int decision; 

    printf("Choose one of the following options: \n");

    printf("Deposit - 1 \n");
    printf("Withdraw - 2 \n");
    printf("Transfer - 3 \n");
    printf("Logout - 4 \n");

    scanf("%d", &decision);

    while(decision != 1 && decision != 2 && decision != 3 && decision != 4){
        printf("Please try again! \n"); 
        scanf("%d", &decision);
    }

    return decision; 
}

void memory_cleanup(user **users, account **accounts){

    user *current_user, *tmp_user;
    HASH_ITER(hh, *users, current_user, tmp_user){
    HASH_DEL(*users, current_user);
    free(current_user);
    }

    account *current_account, *tmp_account;
    HASH_ITER(hh, *accounts, current_account, tmp_account){
        HASH_DEL(*accounts, current_account);
        free(current_account);
    }
}

void handle_program_exit(user **users, account **accounts){
    save_users(users); 
    save_accounts(accounts); 
    memory_cleanup(users, accounts); 
    exit(1); 
}

int main(){
    user *users = NULL; 
    load_users(&users);
    account *accounts = NULL; 
    load_accounts(&accounts); 

    printf("Welcome to your Banking System");

    while(1){
        int decision = auth_menu(); 
        user *user = NULL; 
        switch(decision){
            case 1: user = login_user(&users); break; 
            case 2: user = register_user(&users); break; 
            case 3: handle_program_exit(&users, &accounts);
        }

        if(user != NULL){
            while(1){
                int decision = main_menu(); 
                switch(decision){
                case 1: /* deposit(); */ break;
                case 2: /* withdraw(); */ break;
                case 3: /* transfer(); */ break;
                case 4: logout(&user); break;
                }
            }
        }
    }

    return 0; 
}