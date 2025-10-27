#include "heap.hpp"

int main() {
  char mem[10];

  Block_array b_arr;
  Block arr[20];
  b_arr.ptr = arr;
  b_arr.capacity = 20;

  Heap h(mem, 10, b_arr);

  h.print_free_list();
  void *a = h.Malloc(sizeof(int));
  h.print_free_list();
  h.Free(a);
  h.Merge();
  h.print_free_list();
}
