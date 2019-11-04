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

int parse_args(ParsedInput *input, int argc, char *argv[]) {
  int party;

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
    party = get_party(argv[2]);
    if (party != 1 && party != 2) {
      fprintf(stderr, "party must be either 1 or 2\n");
      print_usage(argv[0]);
      return 1;
    }
    if (is_seller(party) && argc == 3) {
      fprintf(stderr, "file missing\n");
      print_usage(argv[0]);
      return 1;
    }
    else if (!is_seller(party) && argc == 4) {
      fprintf(stderr, "file not needed for receiver\n");
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

  input->party = party;
  input->port = argv[1];

  if (party == 2) {
    input->asset = argv[3];// TODO
  } else {
    input->asset = NULL;
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

  if (input.party == 2) {
    ocTestUtilTcpOrDie(&pd, host, input.port);
  } else {
    ocTestUtilTcpOrDie(&pd, NULL, input.port);
  }
  setCurrentParty(&pd, input.party);

  double init_time = wallClock();

  printf("%d %s %s\n", input.party, input.port, input.asset);
  execYaoProtocol(&pd, asset_swap, &io);

  printf("total time: %lf s\n", wallClock() - init_time);
  cleanupProtocol(&pd);

  return 0;
}
