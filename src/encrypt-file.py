#! /usr/bin/python3

import pyaes
import argparse

STATIC_PADDING = 0
AES_BLOCK_SIZE = 16

def get_args():
    parser = argparse.ArgumentParser(description = "encrypts contents of a file")
    parser.add_argument("--key", type = str, help = "path to file with key")
    parser.add_argument("--plaintext", type = str, help = "path to file with plaintext")
    parser.add_argument("--cipher", type = str, help = "path to file to output ciphertext (will be created/overwritten)")
    return parser.parse_args()

def pad(string, blockSize = AES_BLOCK_SIZE):
    padLen = -len(string) % blockSize
    string += bytes('\0', "ascii") * padLen

    return string

args = get_args()

with open(args.key, 'r') as f:
    key = bytes(f.read(), 'ascii')

with open(args.plaintext, 'rb') as f:
    asset = f.read()

aes = pyaes.AESModeOfOperationECB(key)
paddedAsset = pad(asset, AES_BLOCK_SIZE)
with open(args.cipher, 'wb') as f:
    for i in range(0, len(paddedAsset), AES_BLOCK_SIZE):
        f.write(aes.encrypt(paddedAsset[i:i + AES_BLOCK_SIZE]))
