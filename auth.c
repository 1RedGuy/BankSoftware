#include "banking.h"

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

    char *hex = malloc(41);
    for (int i = 0; i < 20; i++) {
        sprintf(hex + i*2, "%02x", hash[i]);
    }
    hex[40] = '\0';
    return hex;
}

int compare_password(char *password, char *hashed_password) {
    char *hashed = hash_password(password);
    int result = strcmp(hashed, hashed_password);
    free(hashed);
    return result;
}

void load_users(user **users) {
    FILE *file = fopen("data/users.txt", "r");
    if (file == NULL) {
        printf("Users file not found, starting fresh.\n");
        return;
    }

    char line[150];
    while (fgets(line, sizeof(line), file)) {
        user *new_user = malloc(sizeof(user));
        if (!new_user) {
            printf("Memory allocation failed for user.\n");
            continue;
        }
        sscanf(line, "%d %s %s", &new_user->id, new_user->username, new_user->hashed_password);
        HASH_ADD_STR(*users, username, new_user);
        printf("Loaded user: %s (ID %d)\n", new_user->username, new_user->id);
    }

    fclose(file);
}

void save_users(user **users) {
    FILE *file = fopen("data/users.txt", "w");
    if (!file) {
        printf("Error opening users.txt for writing!\n");
        return;
    }

    user *tmp, *tmp2;
    HASH_ITER(hh, *users, tmp, tmp2) {
        fprintf(file, "%d %s %s\n", tmp->id, tmp->username, tmp->hashed_password);
    }
    fclose(file);
    printf("Users saved successfully.\n");
}

void append_user_to_file(user *new_user) {
    FILE *file = fopen("data/users.txt", "a");
    if (!file) {
        printf("Error opening users.txt for append!\n");
        return;
    }
    fprintf(file, "%d %s %s\n", new_user->id, new_user->username, new_user->hashed_password);
    fclose(file);
    printf("Appended new user %s to users.txt\n", new_user->username);
}

int generate_unique_user_id(user **users) {
    int id = 1;
    user *tmp, *tmp2;
    HASH_ITER(hh, *users, tmp, tmp2) {
        if (tmp->id >= id) {
            id = tmp->id + 1;
        }
    }
    return id;
}

user *register_user(user **users, account **accounts){
    char username[MAX_USERNAME_LENGTH];
    char password[MAX_PASSWORD_LENGTH];

    while (1) {
        printf("Enter your username: ");
        scanf("%s", username);

        user *existing = NULL;
        HASH_FIND_STR(*users, username, existing);
        if (existing) {
            printf("Username already exists! Try again.\n");
        } else {
            break;
        }
    }

    printf("Enter your password: ");
    scanf("%s", password);

    user *new_user = malloc(sizeof(user));
    if (!new_user) {
        printf("Memory allocation failed.\n");
        return NULL;
    }

    new_user->id = generate_unique_user_id(users);
    printf("User registered successfully! Your user ID is: %03d\n", new_user->id);
    strcpy(new_user->username, username);

    char *hashed = hash_password(password);
    strcpy(new_user->hashed_password, hashed);
    free(hashed);

    HASH_ADD_STR(*users, username, new_user);
    append_user_to_file(new_user);

    create_account_for_user(accounts, new_user->id);

    printf("User registered successfully!\n");
    return new_user;
}

user *login_user(user **users) {
    char username[MAX_USERNAME_LENGTH];
    char password[MAX_PASSWORD_LENGTH];

    printf("Enter your username: ");
    scanf("%s", username);
    printf("Enter your password: ");
    scanf("%s", password);

    user *found = NULL;
    HASH_FIND_STR(*users, username, found);
    if (found && compare_password(password, found->hashed_password) == 0) {
        printf("Login successful!\n");
        return found;
    }

    printf("Invalid username or password!\n");
    return NULL;
}

void logout(user **user) {
    *user = NULL;
} 