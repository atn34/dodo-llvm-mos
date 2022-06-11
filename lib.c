// Scuffed implementations trying to balance code size and correctness

static unsigned int _seed = 0;
void srand(unsigned int x) { _seed = x; }
int rand() {
  _seed = _seed * 5 + 1;
  return _seed;
}

#define BUFFER_SIZE 32
static char _buffer[BUFFER_SIZE];

char *itoa(int value, int base) {
  char *str = _buffer + BUFFER_SIZE - 1;
  *str-- = 0;
  while (1) {
    *str = '0' + value % base;
    value /= base;
    if (!value) {
      return str;
    }
    --str;
  }
}

extern unsigned char __heap_start[];
unsigned char *first_unallocated = __heap_start;

#pragma clang diagnostic ignored "-Wincompatible-library-redeclaration"

void *malloc(unsigned __size) {
  void *result = first_unallocated;
  first_unallocated += __size;
  return result;
}
void free(void *p) {}