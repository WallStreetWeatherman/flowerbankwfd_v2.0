#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <math.h>
#include <string.h>
#include <winsock2.h>

#pragma comment (lib, "Ws2_32.lib")

#define MAX_ACCOUNTS 10
#define MAX_TRANSACTIONS 100
#define FRAUD_THRESHOLD_AMOUNT 10000.00
#define FRAUD_THRESHOLD_FREQUENCY 3
#define FRAUD_TIME_PERIOD 60 // In seconds

typedef struct {
    int account_number;
    char account_holder[100];
    float balance;
    time_t last_transaction_time;
    int transaction_count;
} BankAccount;

typedef struct {
    int account_number;
    float amount;
    time_t transaction_time;
} Transaction;

// Global variables
BankAccount accounts[MAX_ACCOUNTS]; // Can hold up to MAX_ACCOUNTS for 
Transaction transactions[MAX_TRANSACTIONS]; // Transaction log
int num_accounts = 0;
int num_transactions = 0;

// Function declarations
void create_account(BankAccount *account);
void check_balance(const BankAccount *account);
void deposit(BankAccount *account, float amount);
void withdraw(BankAccount *account, float amount);
bool verify_account(const BankAccount *account, int account_number);
void record_transaction(int account_number, float amount);
bool check_fraud(int account_number, float amount, time_t transaction_time);
void save_accounts(const char* filename);
void load_accounts(const char* filename);
void encrypt_data(unsigned char *plaintext, int plaintext_len, unsigned char *key, unsigned char *iv, unsigned char *ciphertext);
int decrypt_data(unsigned char *ciphertext, int ciphertext_len, unsigned char *key, unsigned char *iv, unsigned char *plaintext);

// Function to serialize account data
void serialize_accounts(char* buffer, int buffer_size) {
    // Example serialization code
    // This is a simple example, you might want to use a more robust serialization approach
    int offset = 0;
    for (int i = 0; i < num_accounts && offset < buffer_size; i++) {
        offset += snprintf(buffer + offset, buffer_size - offset,
                           "Account Number: %d, Holder: %s, Balance: %.2f\n",
                           accounts[i].account_number, accounts[i].account_holder, accounts[i].balance);
    }
}

void deserialize_accounts(const char* buffer) {
    int account_number;
    char account_holder[100];
    float balance;
    char* line = strtok(buffer, "\n");
    while (line != NULL && num_accounts < MAX_ACCOUNTS) {
        if (sscanf(line, "%d,%99[^,],%f", &account_number, account_holder, &balance) == 3) {
            accounts[num_accounts].account_number = account_number;
            strncpy(accounts[num_accounts].account_holder, account_holder, sizeof(account_holder));
            accounts[num_accounts].balance = balance;
            num_accounts++;
        }
        line = strtok(NULL, "\n");
    }
}

void initialize_winsock() {
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (result != 0) {
        printf("WSAStartup failed: %d\n", result);
        exit(1);
    }
}

SOCKET connect_debugger() {
    SOCKET sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == INVALID_SOCKET) {
        printf("Failed to create socket: %ld\n", WSAGetLastError());
        WSACleanup();
        exit(1);
    }

    struct sockaddr_in serveraddr;
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = inet_addr("127.0.0.1");  // localhost
    serveraddr.sin_port = htons(5000);  // Port number

    if (connect(sockfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) == SOCKET_ERROR) {
        printf("Failed to connect: %ld\n", WSAGetLastError());
        closesocket(sockfd);
        WSACleanup();
        exit(1);
    }

    return sockfd;
}

void send_debug_message(SOCKET sockfd, const char* message) {
    send(sockfd, message, strlen(message), 0);
}

// New functions to save and load accounts
void save_accounts(const char* filename) {
    FILE* file = fopen(filename, "w");
    if (file == NULL) {
        perror("Error opening file for writing");
        return;
    }

    // Convert accounts data to a string or a binary format
    // TODO: need to implement a way to serialize 'accounts' into 'data_to_encrypt'
    char data_to_encrypt[4096];
    serialize_accounts(data_to_encrypt, sizeof(data_to_encrypt));

    char command[1024];
    sprintf(command, "python flower_bank_encrypted.py encrypt '%s'", data_to_encrypt);
    FILE* pipe = popen(command, "r");
    if (pipe == NULL) {
        printf("Error calling Python script.\n");
        return;
    }

    char encrypted_data[1024];
    fgets(encrypted_data, sizeof(encrypted_data), pipe);
    pclose(pipe);

    // Save encrypted data to file
    fprintf(file, "%s", encrypted_data);
    fclose(file);
    printf("Accounts have been saved.\n");
}

void load_accounts(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file for reading");
        return;
    }

    char encrypted_data[1024];
    fgets(encrypted_data, sizeof(encrypted_data), file);
    fclose(file);

    char command[1024];
    sprintf(command, "python flower_bank_encrypted.py decrypt '%s'", encrypted_data);
    FILE* pipe = popen(command, "r");
    if (pipe == NULL) {
        printf("Error calling Python script.\n");
        return;
    }

    // Increase the size of decrypted_data to handle larger account data
    char decrypted_data[4096]; // Adjusted size
    fgets(decrypted_data, sizeof(decrypted_data), pipe);
    pclose(pipe);

    // Deserialize the data back into the accounts array
    deserialize_accounts(decrypted_data); // Add this line

    printf("Accounts have been loaded.\n");
}

// Main application entry point
int main() {
    initialize_winsock();
    SOCKET sockfd = connect_debugger();
    send_debug_message(sockfd, "Debugger connected.\n");

    // Load existing accounts
    load_accounts("accounts.dat");

    int choice;
    int entered_account_number;
    float amount;
    bool account_found;
    time_t current_time;

    printf("Welcome to FlowerBank!\n");

    while (true) {
        printf("\nPlease choose an option:\n");
        printf("1. Create a new account\n");
        printf("2. Check account balance\n");
        printf("3. Deposit money\n");
        printf("4. Withdraw money\n");
        printf("5. Exit\n");
        printf("Enter choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                if (num_accounts < 10) {
                    create_account(&accounts[num_accounts]);
                    num_accounts++;
                } else {
                    printf("Bank account limit reached.\n");
                }
                break;
            case 2:
                printf("Enter account number: ");
                scanf("%d", &entered_account_number);
                account_found = false;
                for (int i = 0; i < num_accounts; i++) {
                    if (verify_account(&accounts[i], entered_account_number)) {
                        check_balance(&accounts[i]);
                        account_found = true;
                        break;
                    }
                }
                if (!account_found) {
                    printf("Account not found.\n");
                }
                break;
            case 3:
                printf("Enter account number: ");
                scanf("%d", &entered_account_number);
                printf("Enter amount to deposit: ");
                scanf("%f", &amount);
                account_found = false;
                for (int i = 0; i < num_accounts; i++) {
                    if (verify_account(&accounts[i], entered_account_number)) {
                        deposit(&accounts[i], amount);
                        account_found = true;
                        // After depositing, record the transaction
                        record_transaction(entered_account_number, amount);
                        current_time = time(NULL);
                        if (check_fraud(entered_account_number, amount, current_time)) {
                            printf("Potential fraud detected for account %d!\n", entered_account_number);
                        }
                        break;
                    }
                }
                if (!account_found) {
                    printf("Account not found.\n");
                }
                break;
            case 4:
                printf("Enter account number: ");
                scanf("%d", &entered_account_number);
                printf("Enter amount to withdraw: ");
                scanf("%f", &amount);
                account_found = false;
                for (int i = 0; i < num_accounts; i++) {
                    if (verify_account(&accounts[i], entered_account_number)) {
                        withdraw(&accounts[i], amount);
                        account_found = true;
                         record_transaction(entered_account_number, -amount); // Negative for withdrawals
                        current_time = time(NULL);
                        if (check_fraud(entered_account_number, amount, current_time)) {
                            printf("Potential fraud detected for account %d!\n", entered_account_number);
                        }
                        break;
                    }
                }
                if (!account_found) {
                    printf("Account not found.\n");
                }
                break;
            case 5:
                save_accounts("accounts.dat");
                printf("Thank you for using FlowerBank. Goodbye!\n");
                return 0;
            default:
                printf("Invalid option. Please try again.\n");
                break;
        }
    }

    closesocket(sockfd);  // Close the socket when done
    WSACleanup();  // Cleanup Winsock
    return 0;
}

void create_account(BankAccount *account) {
    printf("Enter the account number: ");
    scanf("%d", &account->account_number);
    printf("Enter the name of the account holder: ");
    scanf("%s", account->account_holder);
    account->balance = 0.0f;
    printf("Account created successfully!\n");
}

void check_balance(const BankAccount *account) {
    printf("The balance of account %d (%s) is $%.2f\n", account->account_number, account->account_holder, account->balance);
}

void deposit(BankAccount *account, float amount) {
    if (amount < 0) {
        printf("Invalid amount. Please enter a positive number.\n");
    } else {
        account->balance += amount;
        printf("Successfully deposited $%.2f to account %d (%s).\n", amount, account->account_number, account->account_holder);
    }
}

void withdraw(BankAccount *account, float amount) {
    if (amount < 0) {
        printf("Invalid amount. Please enter a positive number.\n");
    } else if (amount > account->balance) {
        printf("Insufficient funds. Transaction cancelled.\n");
    } else {
        account->balance -= amount;
        printf("Successfully withdrew $%.2f from account %d (%s).\n", amount, account->account_number, account->account_holder);
    }
}

bool verify_account(const BankAccount *account, int account_number) {
    return account->account_number == account_number;
}

void record_transaction(int account_number, float amount) {
    if (num_transactions >= MAX_TRANSACTIONS) {
        printf("Transaction log is full. Oldest transactions will not be recorded.\n");
        return;
    }
    transactions[num_transactions].account_number = account_number;
    transactions[num_transactions].amount = amount;
    transactions[num_transactions].transaction_time = time(NULL);
    num_transactions++;
}

bool check_fraud(int account_number, float amount, time_t transaction_time) {
    int frequency = 0;
    for (int i = 0; i < num_transactions; i++) {
        if (transactions[i].account_number == account_number) {
            // Check for high frequency of transactions
            if (difftime(transaction_time, transactions[i].transaction_time) <= FRAUD_TIME_PERIOD) {
                frequency++;
            }
            // Check for a large transaction amount
            if (fabs(transactions[i].amount) >= FRAUD_THRESHOLD_AMOUNT) {
                return true; // Large transaction could be potential fraud
            }
        }
    }
    if (frequency > FRAUD_THRESHOLD_FREQUENCY) {
        return true; // High frequency of transactions could be potential fraud
    }
    return false; // No fraud detected
}
