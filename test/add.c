#include <obliv.h>
#include <stdio.h>
#include <time.h>
#include <ctype.h>

#include "../obliv-c/test/oblivc/common/util.h"
#include "add.h"

char *host = "localhost";

int check_args(int argc, char *argv[]) {
  if (argc < 4) {
    if (argc == 1) {
      fprintf(stderr, "port number missing\n");
    } else if (argc == 2) {
      fprintf(stderr, "host missing\n");
    } else { // argc == 3
      fprintf(stderr, "string missing\n");
    }

    fprintf(stderr, "usage: %s <port> <party number: 1|2> <summand>\n", argv[0]);
    fprintf(stderr, "  each party must provide one decimal number\n");
    return 1;
  }

  for (int i = 1; i < 4; i++) {
    for (int j = 0; argv[i][j] != '\0'; j++) {
      if (!isdigit(argv[i][j])) {
        fprintf(stderr, "Argument %d not a number: %c not a digit", i, argv[i][j]);
        return 1;
      }
    }
  }

  int party = (int) strtol(argv[2], NULL, 10);
  if (party != 1 && party != 2) {
    return 1;
  }

  return 0;
}

int test_add(int party, ProtocolIO *io) {
  int expected = io->summands[0] + io->summands[1];
  if (io->sum != expected) {
    fprintf(stderr, "party %d: expected %d, got %d\n", party, expected, io->sum);
    return 1;
  }
  return 0;
}

int main(int argc, char *argv[]) {
  ProtocolDesc pd;
  ProtocolIO io;

  if (check_args(argc, argv)) {
    return 1;
  }

  int party = (int) strtol(argv[2], NULL, 10);

  if (party == 2) {
    ocTestUtilTcpOrDie(&pd, host, argv[1]);
  } else {
    ocTestUtilTcpOrDie(&pd, NULL, argv[1]);
  }
  io.summands[party - 1] = (int) strtol(argv[3], NULL, 10);
  setCurrentParty(&pd, party);

  double init_time = wallClock();

  execYaoProtocol(&pd, add, &io);

  printf("total time: %lf s\n", wallClock() - init_time);
  cleanupProtocol(&pd);

  if (test_add(party, &io)) {
    return 1;
  }

  return 0;
}
