#include <stdio.h>
#include <stdbool.h>
#include <time.h>

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
BankAccount accounts[MAX_ACCOUNTS]; // Can hold up to MAX_ACCOUNTS for this example
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

// Main application entry point
int main() {
    int choice;
    int entered_account_number;
    float amount;
    bool account_found;
    time_t current_time;

    printf("Welcome to SimpleBank!\n");

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
                printf("Thank you for using SimpleBank. Goodbye!\n");
                return 0;
            default:
                printf("Invalid option. Please try again.\n");
                break;
        }
    }

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
