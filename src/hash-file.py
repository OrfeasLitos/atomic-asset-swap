#! /usr/bin/python3

from hashlib import sha256
import argparse

def get_args():
    parser = argparse.ArgumentParser(description = "hashes contents of a file")
    parser.add_argument("--preimage", type = str, help = "path to file with preimage")
    parser.add_argument("--hash", type = str, help = "path to file to output hash (will be created/overwritten)")
    return parser.parse_args()

args = get_args()

with open(args.preimage, 'rb') as f:
    preimage = f.read()

with open(args.hash, 'wb') as f:
    f.write(sha256(preimage).digest())
