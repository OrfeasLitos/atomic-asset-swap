#pragma once

#define SECRET_SIZE 32
#define SECRET_HASH_SIZE 32

#define MAX_ASSET_PLAIN_SIZE 1024
#define MAX_ASSET_CIPHER_SIZE 1024
#define ASSET_HASH_SIZE 32

#define KEY_SIZE 32
#define KEY_HASH_SIZE 32

extern const int SELLER;
extern const int BUYER;

typedef struct {
  char *secret;
  char *secret_hash;

  char *asset_plain;
  int asset_plain_size;
  char *asset_cipher;
  int asset_cipher_size;
  char *asset_hash;

  char *key;
  char *key_hash;
} ProtocolIO;

void asset_swap(void *arg);
