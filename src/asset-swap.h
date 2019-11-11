#pragma once

#define SECRET_SIZE 32
#define SECRET_HASH_SIZE 32

#define MAX_ASSET_PLAIN_SIZE 1024
#define MAX_ASSET_CIPHER_SIZE 1024
#define ASSET_HASH_SIZE 32

#define KEY_SIZE 16
#define KEY_HASH_SIZE 32

extern const int SELLER;
extern const int BUYER;

typedef struct {
  unsigned char secret[SECRET_SIZE];
  unsigned char *secret_hash;

  unsigned char *asset_plain;
  int asset_plain_size;
  unsigned char *asset_cipher;
  int asset_cipher_size;
  unsigned char *asset_hash;

  unsigned char *key;
  unsigned char *key_hash;
} ProtocolIO;

void asset_swap(void *arg);
