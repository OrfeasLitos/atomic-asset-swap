#pragma once

typedef struct {
  char *secret;
  char *secret_hash;

  char *asset_plain;
  char *asset_cipher;
  char *asset_hash;

  char *key;
  char *key_hash;
} ProtocolIO;

void asset_swap(void *arg);
