import pyaes
import argparse
from hashlib import sha256

secret = "secret"

## This class simplifies key derivation and keeping track of counters
class AES:
    ## 256-bit key derivation for AES using SHA256
    def __init__(self, key):
        self.key = sha256(bytes(str(key), 'ascii')).digest()
        self.encrypt_counter = self.decrypt_counter = 0

    def encrypt(self, plaintext):
        counter = pyaes.Counter(initial_value = self.encrypt_counter)
        aes = pyaes.AESModeOfOperationCTR(self.key, counter = counter)
        self.encrypt_counter += len(plaintext)
        return aes.encrypt(plaintext)

    def decrypt(self, ciphertext):
        counter = pyaes.Counter(initial_value = self.decrypt_counter)
        aes = pyaes.AESModeOfOperationCTR(self.key, counter = counter)
        self.decrypt_counter += len(ciphertext)
        return aes.decrypt(ciphertext).decode()

parser = argparse.ArgumentParser(description = "encrypts contents of a file")
parser.add_argument("--input", type = str, help = "path to file with plaintext")
parser.add_argument("--output", type = str, help = "path to file to output ciphertext (will be created/overwritten)")
args = parser.parse_args()

with open(args.input, 'r') as f:
    asset = f.read()

with open(args.output, 'wb') as f:
    f.write(AES(secret).encrypt(asset))
