#include <obliv.h>
#include <stdio.h>
#include <time.h>
#include <ctype.h>

#include "../obliv-c/test/oblivc/common/util.h"
#include "asset-swap.h"

const unsigned char *host = "localhost";
const size_t SELLER = 2;
const size_t BUYER  = 1;

int get_party(unsigned char *input) {
  return (int) strtol(input, NULL, 10);
}

void print_usage(char *bin_name) {
  fprintf(stderr, "\nusage: %s <port> <party_number: 1|2>"
                  " ?<file> ?<file> ?<file>\n", bin_name);
  fprintf(stderr, "    for <party_number> \"1\" (buyer), add "
                  "<cipher_file> <asset_hash_file> <key_hash_file>\n");
  fprintf(stderr, "    for <party_number> \"2\" (seller), "
                  "add <asset_file> <key_file>\n\n");
}

bool invalid_argc(party, argc) {
  if (party != SELLER && party != BUYER) {
    return false;
  }

  return (party == SELLER && argc != 5)
      || (party == BUYER && argc != 6);
}

int check_args(int *party, int argc, char *argv[]) {
  if (argc >= 3) {
    *party = get_party(argv[2]);
  }

  if (invalid_argc(*party, argc)) {
    if (argc == 1) {
      fprintf(stderr, "port number, role, files missing\n");
    } else if (argc == 2) {
      fprintf(stderr, "role, files missing\n");
    } else {
      if (*party != BUYER && *party != SELLER) {
        fprintf(stderr, "party must be either 1 or 2\n");
      }
      if (argc == 3) {
        fprintf(stderr, "filenames missing\n");
        if (*party == BUYER) {
          fprintf(stderr, "party 1 (buyer) must have a "
                          "<cipher_file>, an <asset_hash_file> "
                          "and a <key_hash_file>\n");
        } else if (*party == SELLER) {
          fprintf(stderr, "party 2 (seller) must have an "
                          "<asset_file> and a <key_file>\n");
        }
      } else if (argc == 4) {
        fprintf(stderr, "second filename missing\n");
        if (*party == BUYER) {
          fprintf(stderr, "party 1 (buyer) misses an <asset_hash_file>"
                          " and a <key_hash_file>\n");
        } else if (*party == SELLER) {
          fprintf(stderr, "party 2 (seller) misses a <key_file>\n");
        }
      } else if (*party == BUYER && argc == 5) {
        fprintf(stderr, "party 1 (buyer) misses a <key_hash_file>\n");
      } else {
        fprintf(stderr, "too many arguments\n");
      }
    }

    print_usage(argv[0]);
    return 1;
  }

  for (int i = 1; i < 3; i++) {
    for (int j = 0; argv[i][j] != '\0'; j++) {
      if (!isdigit(argv[i][j])) {
        fprintf(stderr, "Argument %d not a number: "
                        "%c not a digit\n\n", i, argv[i][j]);
        print_usage(argv[0]);
        return 1;
      }
    }
  }

  return 0;
}

// https://stackoverflow.com/questions/3747086/reading-the-whole-text-file-into-a-char-array-in-c
int read_file(unsigned char **buf, long *file_size, char *file_name) {
  FILE *fp;

  fp = fopen(file_name, "rb");
  if (!fp) {
    fprintf(stderr, "Could not open file %s\n", file_name);
    return 1;
  }

  fseek(fp, 0L, SEEK_END);
  *file_size = ftell(fp);
  rewind(fp);

  /* allocate memory for entire content */
  *buf = malloc((*file_size + 1) * sizeof(unsigned char));
  if (!(*buf)) {
    fclose(fp);
    fprintf(stderr, "memory allocation failed\n");
    return 1;
  }

  /* copy the file into the buffer */
  if(1 != fread(*buf, *file_size, 1, fp)) {
    fclose(fp);
    free(*buf);
    fprintf(stderr, "reading from file %s failed\n", file_name);
    return 1;
  }

  (*buf)[*file_size] = '\0';

  fclose(fp);
  return 0;
}

int parse_args(ParsedInput *input, int argc, char *argv[]) {
  if (check_args(&(input->party), argc, argv)) {
    return 1;
  }

  input->port = argv[1];

  if (input->party == SELLER) {
    if (read_file(&(input->asset), &(input->asset_size), argv[3])) {
      return 1;
    }

    long key_size;
    if (read_file(&(input->key), &key_size, argv[4])) {
      return 1;
    }
    if (key_size != KEY_SIZE) {
      fprintf(stderr, "got key of size %d, should "
                      "be %d bytes\n", key_size, KEY_SIZE);
      return 1;
    }
  } else { // BUYER
    if (read_file(&(input->cipher), &(input->cipher_size), argv[3])) {
      return 1;
    }

    long asset_hash_size;
    if (read_file(&(input->expected_asset_hash), &asset_hash_size, argv[4])) {
      return 1;
    }
    if (asset_hash_size != ASSET_HASH_SIZE) {
      fprintf(stderr, "got asset hash of size %d, should "
                      "be %d bytes\n", asset_hash_size, ASSET_HASH_SIZE);
      return 1;
    }

    long key_hash_size;
    if (read_file(&(input->expected_key_hash), &key_hash_size, argv[5])) {
      return 1;
    }
    if (key_hash_size != KEY_HASH_SIZE) {
      fprintf(stderr, "got key hash of size %d, should "
                      "be %d bytes\n", key_hash_size, KEY_HASH_SIZE);
      return 1;
    }
  }

  return 0;
}

int main(int argc, char *argv[]) {
  ProtocolDesc pd;
  ProtocolIO io;
  ParsedInput input;

  if (parse_args(&input, argc, argv)) {
    return 1;
  }

  if (input.party == SELLER) {
    io.asset_plain = input.asset;
    io.asset_plain_size = input.asset_size;
    io.key = input.key;
  } else { // BUYER
    io.expected_asset_hash = input.expected_asset_hash;
    io.asset_cipher = input.cipher;
    io.asset_cipher_size = input.cipher_size;
    io.expected_key_hash = input.expected_key_hash;
  }

  if (input.party == BUYER) {
    ocTestUtilTcpOrDie(&pd, NULL, input.port);
  } else {
    ocTestUtilTcpOrDie(&pd, host, input.port);
  }
  setCurrentParty(&pd, input.party);

  double init_time = wallClock();

  execYaoProtocol(&pd, asset_swap, &io);

  printf("total protocol time: %lf s\n", wallClock() - init_time);

  cleanupProtocol(&pd);

  free(input.asset);

  return 0;
}
