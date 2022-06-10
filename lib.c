// Scuffed implementations trying to balance code size and correctness

static unsigned int _seed = 0;
void srand(unsigned int x) { _seed = x; }
int rand() {
  _seed = _seed * 5 + 1;
  return _seed;
}

char *itoa(int value, char *str, int base) {
  str += 9;
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