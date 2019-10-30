#pragma once

typedef struct {
  int summands[2];
  int sum;
} ProtocolIO;

void add(void *arg);
