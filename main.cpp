#include "heap.hpp"

int main() {
  char mem[1024];

  Block_array b_arr;
  Block arr[20];
  b_arr.ptr = arr;
  b_arr.capacity = 20;

  Heap h(mem, 1024, b_arr);

  h.print_free_list();
  h.Malloc(10);
  h.print_free_list();
  h.Malloc(123);
  h.print_free_list();
}
