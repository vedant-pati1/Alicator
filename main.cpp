#include "heap.hpp"
#include <stdatomic.h>

int main() {
  char mem[1024];

  Block_array b_arr;
  Block arr[20];
  b_arr.ptr = arr;
  b_arr.capacity = 20;

  Heap h(mem, 1024, b_arr);

  h.print_free_list();
  void *a = h.Malloc(sizeof(int));
  h.print_free_list();
  h.Free(a);
  h.print_free_list();
}
