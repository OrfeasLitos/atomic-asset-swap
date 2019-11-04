#include <obliv.h>
#include <stdio.h>
#include <time.h>
#include <ctype.h>

#include "../obliv-c/test/oblivc/common/util.h"
#include "asset-swap.h"

char *host = "localhost";

typedef struct {
  int party;
  char *port;
  char *asset;
} ParsedInput;

int get_party(char *input) {
  return (int) strtol(input, NULL, 10);
}

bool is_seller(int party) {
  return party == 2;
}

void print_usage(char *bin_name) {
  fprintf(stderr, "usage: %s <port> <party number: 1|2> <if party == 2: file>\n", bin_name);
  fprintf(stderr, "  seller (party 2) must provide file name of digital asset\n");
}

int check_args(int *party, int argc, char *argv[]) {
  if (argc < 3) {
    if (argc == 1) {
      fprintf(stderr, "port number, role missing\n");
    } else { // argc == 2
      fprintf(stderr, "role missing\n");
    }

    print_usage(argv[0]);
    return 1;
  }
  else if (argc > 4) {
    print_usage(argv[0]);
    return 1;
  }
  else {
    *party = get_party(argv[2]);
    if (*party != 1 && *party != 2) {
      fprintf(stderr, "party must be either 1 or 2\n");
      print_usage(argv[0]);
      return 1;
    }
    if (is_seller(*party) && argc == 3) {
      fprintf(stderr, "file missing in seller\n");
      print_usage(argv[0]);
      return 1;
    }
    else if (!is_seller(*party) && argc == 4) {
      fprintf(stderr, "file not needed in receiver\n");
      print_usage(argv[0]);
      return 1;
    }
  }

  for (int i = 1; i < 3; i++) {
    for (int j = 0; argv[i][j] != '\0'; j++) {
      if (!isdigit(argv[i][j])) {
        fprintf(stderr, "Argument %d not a number: %c not a digit", i, argv[i][j]);
        print_usage(argv[0]);
        return 1;
      }
    }
  }

  return 0;
}

// https://stackoverflow.com/questions/3747086/reading-the-whole-text-file-into-a-char-array-in-c
int read_file(char **buf, char *file_name) {
  FILE *fp;
  long file_size;

  fp = fopen(file_name, "rb");
  if (!fp) {
    fprintf(stderr, "Could not open file %s\n", file_name);
    return 1;
  }

  fseek(fp, 0L, SEEK_END);
  file_size = ftell(fp);
  rewind(fp);

  /* allocate memory for entire content */
  *buf = malloc((file_size + 1) * sizeof(char));
  if (!(*buf)) {
    fclose(fp);
    fprintf(stderr, "memory allocation failed\n");
    return 1;
  }

  /* copy the file into the buffer */
  if(1 != fread(*buf, file_size, 1, fp)) {
    fclose(fp);
    free(*buf);
    fprintf(stderr, "reading from file %s failed\n", file_name);
    return 1;
  }

  buf[file_size] = '\0';

  fclose(fp);
  return 0;
}

int parse_args(ParsedInput *input, int argc, char *argv[]) {
  if (check_args(&(input->party), argc, argv)) {
    return 1;
  }

  input->port = argv[1];

  if (input->party == 2) {
    if (read_file(&(input->asset), argv[3])) {
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

  // do all printing before the connection starts
  // or expect weird TCP failures...
  if (input.party == 1) {
    printf("party: %d, port: %s\n", input.party, input.port);
  } else {
    printf("party: %d, port: %s, filename: %s, contents: %s\n",
           input.party, input.port, argv[3], input.asset);
  }

  if (input.party == 1) {
    ocTestUtilTcpOrDie(&pd, NULL, input.port);
  } else {
    ocTestUtilTcpOrDie(&pd, host, input.port);
  }
  setCurrentParty(&pd, input.party);

  double init_time = wallClock();

  execYaoProtocol(&pd, asset_swap, &io);

  printf("total time: %lf s\n", wallClock() - init_time);
  cleanupProtocol(&pd);

  free(input.asset);

  return 0;
}
