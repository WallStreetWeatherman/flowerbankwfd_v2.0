#include <stdio.h>
#include <stdbool.h>

typedef struct {
    int account_number;
    char account_holder[100];
    float balance;
} BankAccount;

// Function declarations
void create_account(BankAccount *account);
void check_balance(const BankAccount *account);
void deposit(BankAccount *account, float amount);
void withdraw(BankAccount *account, float amount);
bool verify_account(const BankAccount *account, int account_number);

int main() {
    BankAccount accounts[10]; // Can hold up to 10 accounts for this example
    int num_accounts = 0;
    int choice;
    int entered_account_number;
    float amount;
    bool account_found;

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
