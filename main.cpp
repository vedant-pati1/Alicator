#include "heap.hpp"

int main() {
  char mem[20];

  Block_array b_arr;
  Block arr[20];

  b_arr.ptr = arr;
  b_arr.capacity = 20;

  Heap h(mem, 20, b_arr);
  std::cout << "Initial Free List" << std::endl;
  h.print_free_list();
  void *a = h.Malloc(sizeof(int));
  std::cout << "After allocation of int" << std::endl;
  h.print_free_list();
  h.Free(a);
  std::cout << "Aftering freeing that int" << std::endl;
  h.print_free_list();
  h.merge_free_list();
  std::cout << "After merging the free list" << std::endl;
  h.print_free_list();
}
