#include <cstddef>
#include <iostream>

struct Block {
  // single block consist of multiple chunks
  void *start;
  size_t size;
};
struct Block_array {
  // array of block
  Block *ptr;
  size_t size;
  size_t capacity; // as its memory is allocated by the user there has to be
                   // upper limit to the size
};

class Heap {

private:
  static constexpr size_t chunk_size = sizeof(char *);
  char *heap;
  size_t heap_capacity;
  size_t heap_size;
  Block_array free_list;

public:
  Heap(char *ptr, size_t capacity, Block_array block_list,
       size_t block_capacity) {
    // user will provide memory
    //  by default heap is not present on embedded system is no OS
    //  so we cannot use malloc or new to get memory

    heap = ptr;
    heap_size = 0;

    if (capacity % chunk_size == 0)
      heap_capacity = capacity;
    else {
      heap_capacity = capacity - (capacity % chunk_size);
    }

    this->free_list = block_list;
    // since heap is empty
    free_list.ptr[0].start = heap;
    free_list.ptr[0].size = heap_capacity;
    free_list.size = 1;
    free_list.capacity = block_capacity;
  }
  ~Heap() {
    // erasing all the data from our heap
    for (size_t i = 0; i < heap_capacity; i++) {
      heap[i] = 0;
    }
  }
  void print_free_list() {
    int count = 0;
    for (Block *ptr = free_list.ptr; ptr < (free_list.ptr + free_list.size);
         ptr++)
      std::cout << "Address of block " << count << " : " << ptr->start
                << " Size: " << ptr->size << std::endl;
    count++;
  }
};

int main() {
  char mem[1024];
  Block_array b_arr;
  Block arr[1024];
  b_arr.ptr = arr;
  b_arr.capacity = 1024;
  Heap h(mem, 1024, b_arr, 1024);
  h.print_free_list();
}
