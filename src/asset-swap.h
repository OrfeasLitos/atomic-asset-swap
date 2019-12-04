#pragma once

#define SECRET_SIZE 32
#define SECRET_HASH_SIZE 32

#define ASSET_PLAIN_SIZE 1024
#define ASSET_CIPHER_SIZE 1024
#define ASSET_HASH_SIZE 32

#define KEY_SIZE 16
#define KEY_HASH_SIZE 32

#define AES_BLOCK_SIZE 16

extern const size_t SELLER;
extern const size_t BUYER;

typedef struct {
  unsigned char secret[SECRET_SIZE];
  unsigned char *secret_hash;

  unsigned char *asset_plain;
  int asset_plain_size;
  unsigned char *asset_cipher;
  int asset_cipher_size;
  unsigned char *expected_asset_hash;

  unsigned char *key;
  unsigned char *expected_key_hash;
} ProtocolIO;

typedef struct {
  int party;
  unsigned char *port;

  unsigned char *asset;
  int asset_size;
  unsigned char *expected_asset_hash;
  unsigned char *expected_key_hash;
  unsigned char *cipher;
  int cipher_size;

  unsigned char *key;
} ParsedInput;

void asset_swap(void *arg);
