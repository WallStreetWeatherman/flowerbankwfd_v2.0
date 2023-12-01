# flower_bank_encrypted.py
import os
from cryptography.fernet import Fernet
import sys

def get_fernet_key():
    key = os.environ.get("FlowerBankEncryptionKey")
    if not key:
        raise Exception("Fernet key not found in environment variables")
    return key.encode()

def encrypt(data, key):
    fernet = Fernet(key)
    return fernet.encrypt(data.encode()).decode()

def decrypt(data, key):
    fernet = Fernet(key)
    return fernet.decrypt(data.encode()).decode()

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: python flower_bank_encrypted.py <encrypt|decrypt> <data>")
        sys.exit(1)

    operation = sys.argv[1]
    data = sys.argv[2]
    key = get_fernet_key()

    if operation == "encrypt":
        print(encrypt(data, key))
    elif operation == "decrypt":
        print(decrypt(data, key))
