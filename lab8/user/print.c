#include <ulib.h>
#define printf(...) fprintf(1, __VA_ARGS__)

int main(int argc, char **argv) {
  for (int i = 1; i < argc; i++) {
    printf(" %s ", argv[i]);
  }
  printf("\n");
  return 0;
}
