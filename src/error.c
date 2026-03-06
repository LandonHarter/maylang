#include "error.h"
#include "token.h"
#include <stdio.h>
#include <stdlib.h>

void throw_runtime_error(const char* message) {
   fprintf(stderr, "May Runtime Error: %s\n", message);
   exit(1);
}
