#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX_TRANSACTIONS 100

typedef struct {
    int account_id;
    double transaction_amount;
    time_t transaction_time;
} Transaction;

Transaction transaction_log[MAX_TRANSACTIONS];

int transaction_count = 0;

void log_transaction(int account_id, double amount, time_t trans_time);
int check_fraud(Transaction *transaction_log, int count);

int main() {
    // Simulate transaction logging
    srand(time(NULL)); // Seed for random number generation
    
    // Logging transactions
    for (int i = 0; i < MAX_TRANSACTIONS; i++) {
        int account_id = rand() % 10; // Simulate 10 different accounts
        double amount = (double)rand() / RAND_MAX * 1000; // Random transaction amount
        time_t trans_time = time(NULL) - rand() % 300; // Random time within the last 5 minutes
        log_transaction(account_id, amount, trans_time);
    }
    
    // Check for fraud in the logged transactions
    int fraud_index = check_fraud(transaction_log, transaction_count);
    if (fraud_index != -1) {
        printf("Potential fraud detected! Transaction ID: %d\n", fraud_index);
    } else {
        printf("No potential fraud detected.\n");
    }
    
    return 0;
}

void log_transaction(int account_id, double amount, time_t trans_time) {
    if (transaction_count >= MAX_TRANSACTIONS) {
        printf("Transaction log is full\n");
        return;
    }
    transaction_log[transaction_count].account_id = account_id;
    transaction_log[transaction_count].transaction_amount = amount;
    transaction_log[transaction_count].transaction_time = trans_time;
    transaction_count++;
}

int check_fraud(Transaction *transaction_log, int count) {
    const double FRAUD_THRESHOLD = 1000.00; // Threshold for a single transaction to be considered as potential fraud
    const int TIME_THRESHOLD = 60; // Time in seconds for repeated transactions to be considered suspicious
    const double REPEATED_TRANSACTION_THRESHOLD = 3; // Number of repeated transactions considered suspicious
    
    for (int i = 0; i < count; i++) {
        if (transaction_log[i].transaction_amount > FRAUD_THRESHOLD) {
            // Single high-value transaction could be potential fraud
            return i;
        }
        
        // Check for repeated transactions within a short time frame
        int repeated_transactions = 1;
        for (int j = i + 1; j < count; j++) {
            if (transaction_log[j].account_id == transaction_log[i].account_id &&
                difftime(transaction_log[j].transaction_time, transaction_log[i].transaction_time) < TIME_THRESHOLD) {
                repeated_transactions++;
            }
        }
        
        if (repeated_transactions >= REPEATED_TRANSACTION_THRESHOLD) {
            // Repeated transactions could be potential fraud
            return i;
        }
    }
    return -1; // No fraud detected
}
