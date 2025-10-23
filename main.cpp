#include <cassert>
#include <cstddef>
#include <iostream>

struct Block {
  // single block consist of multiple chunks, each chunk is of 1 byte (one char)
  char *start;
  size_t size;
};
struct Block_array {
  // array of block
  Block *ptr;
  size_t size;
  size_t capacity; // as its memory is allocated by the user there has to be
                   // upper limit to the size

  int get_block_index(Block *block) {
    for (size_t i = 0; i < size; i++) {
      if (block == (ptr + i))
        return i;
    }
    return -1;
  }

  void remove_block(Block *block) {

    int index = get_block_index(block);
    if (index == -1)
      return;
    for (size_t i = index; i < size - 1; i++) {
      ptr[i] = ptr[i + 1];
    }

    // size can never be -ve
    assert(size > 0);
    size--;
  }

  void merge_block() {
    std::cout << "not implemented yet";
    assert(false);
  }
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
         ptr++) {

      std::cout << "Address of block " << count << " : " << (void *)ptr->start
                << " Size: " << ptr->size << std::endl;
      count++;
    }
  }
  void *Malloc(size_t size) {
    if (size > heap_capacity - heap_size) {
      std::cout << "Heap is full, cannot allocated this much memory";
      return nullptr;
    }
    if (size == 0)
      return nullptr;

    //  iterate through free list and find chunk that fits our requirement
    //  if chunk is of greater size than take required memory and reduce the
    //  size of the block in the free_list

    for (size_t i = 0; i < free_list.size; i++) {
      if (free_list.ptr[i].size == size) {
        // removed the block from free list
        char *tmp = free_list.ptr[i].start;
        free_list.remove_block(&free_list.ptr[i]);
        return tmp;
      } else if (free_list.ptr[i].size > size) {
        char *tmp = free_list.ptr[i].start;

        free_list.ptr[i].start = tmp + size;
        free_list.ptr[i].size = free_list.ptr[i].size - size;

        return tmp;
      }
    }
    // now we have to merge some blocks if there is no readily avaiable block of
    // >=size
    free_list.merge_block(); // not implemented yet

    // then try again to find
    for (size_t i = 0; i < free_list.size; i++) {
      if (free_list.ptr[i].size == size) {
        // removed the block from free list
        char *tmp = free_list.ptr[i].start;
        free_list.remove_block(&free_list.ptr[i]);
        return tmp;
      } else if (free_list.ptr[i].size > size) {
        char *tmp = free_list.ptr[i].start;

        free_list.ptr[i].start = tmp + size;
        free_list.ptr[i].size = free_list.ptr[i].size - size;

        return tmp;
      }
    }
    std::cout << "Algorithm inefficent to resolve the fragmentation";
    return nullptr;
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
