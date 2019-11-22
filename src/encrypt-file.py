#! /usr/bin/python3

import pyaes
import argparse

parser = argparse.ArgumentParser(description = "encrypts contents of a file")
parser.add_argument("--key", type = str, help = "path to file with key")
parser.add_argument("--plaintext", type = str, help = "path to file with plaintext")
parser.add_argument("--cipher", type = str, help = "path to file to output ciphertext (will be created/overwritten)")
args = parser.parse_args()

with open(args.key, 'r') as f:
    key = bytes(f.read(), 'ascii')

with open(args.plaintext, 'r') as f:
    asset = f.read()

with open(args.cipher, 'wb') as f:
    aes = pyaes.AESModeOfOperationECB(key)
    f.write(aes.encrypt(asset))
