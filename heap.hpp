#include <cassert>
#include <cstddef>
#include <cstdlib>
#include <filesystem>
#include <iostream>

using std::cout;
using std::endl;

struct Block {
  //  single block consist of multiple chunks, each chunk is of 1 byte (one
  //  char)
  char *start;
  size_t size;

  Block(char *start, size_t size) : start(start), size(size) {}
  Block() : start(nullptr), size(0) {}
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
  void print_block_arr() {
    int count = 0;
    cout << "==========================" << endl;
    cout << "Block Array" << endl;
    for (size_t i = 0; i < size; i++) {
      cout << "       Block " << count
           << " start address: " << (void *)ptr[i].start
           << " Size: " << ptr[i].size << std::endl;
      count++;
    }
    std::flush(std::cout);
  }
  Block *find_block_by_start_ptr(char *ptr) {

    for (size_t i = 0; i < size; i++) {
      if (this->ptr[i].start == ptr) {
        return &(this->ptr[i]);
      }
    }
    cout << "block with this start ptr does not exist in this block array";
    return nullptr;
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

  void insert_block(Block *block) {
    if (size + 1 > capacity) {
      cout << "Block array is full" << endl;
      return;
    }
    if (size == 0) {
      // handle size == 0 separately, becasue size-1 for type size_t overflows
      ptr[0].size = block->size;
      ptr[0].start = block->start;
      size++;
      return;
    }

    // int count = 0;
    // Block *tmp = ptr;
    // while (tmp[count].start <= block->start) {
    // if (tmp->start == block->start) {
    // cout << "block with same start value already exists in this block array"
    //<< endl;
    // return;
    //}
    // count++;
    //}
    // for (size_t i = size; i > size - count - 1; i--) {
    // ptr[i] = ptr[i - 1];
    //}
    //
    // ptr[size - count - 1].start = block->start;
    // ptr[size - count - 1].size = block->size;
    //
    // size++;
    size_t insertion_idx = 0;
    while (insertion_idx < size) {
      if (ptr[insertion_idx].start == block->start) {
        cout << "block with same start value already exists in this block array"
             << endl;
        return;
      } else if (ptr[insertion_idx].start > block->start)
        break;

      insertion_idx++;
    }
    for (size_t i = size; i > insertion_idx; i--) {

      ptr[i] = ptr[i - 1];
    }
    ptr[insertion_idx].start = block->start;
    ptr[insertion_idx].size = block->size;
    size++;
  }

  void update_block(Block *block, char *ptr, size_t size) {

    int idx = this->get_block_index(block);
    if (idx != -1) {
      block->start = ptr;
      block->size = size;
    } else {
      cout << "This block does not exist in this block array";
    }
  }

  void merge_blocks() {
    // we need to merge adjacent blocks so that we get less divison in blocks
    // and size of themerged block will be bigger.
    if (size < 2)
      return;
    size_t i = 0;
    while (i < size - 1) {
      if ((ptr[i].start + ptr[i].size) == ptr[i + 1].start) {
        // they can be merged
        ptr[i].size += ptr[i + 1].size;
        remove_block(&ptr[i + 1]);
      } else {
        i++;
      }
    }
  }
};

class Heap {

private:
  static constexpr size_t chunk_size = sizeof(char *);
  char *heap;
  size_t heap_capacity;
  size_t heap_size;
  Block_array free_list;
  Block_array alloc_list; // list of allocted blocks

public:
  Heap(char *ptr, size_t capacity, Block_array block_list) {
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

    free_list.ptr = block_list.ptr;
    free_list.capacity = block_list.capacity / 2;
    free_list.ptr[0].start = heap;
    free_list.ptr[0].size = heap_capacity;
    free_list.size = 1;

    alloc_list.ptr = (free_list.ptr + free_list.capacity);
    alloc_list.capacity = block_list.capacity - free_list.capacity;
    alloc_list.size = 0;
  }
  ~Heap() {
    // erasing all the data from our heap
    for (size_t i = 0; i < heap_capacity; i++) {
      heap[i] = 0;
    }
  }

  void print_free_list() {
    int count = 0;
    cout << "==========================" << endl;
    cout << "free list" << endl;
    for (size_t i = 0; i < free_list.size; i++) {
      cout << "       Block " << count
           << " start address: " << (void *)free_list.ptr[i].start
           << " Size: " << free_list.ptr[i].size << std::endl;
      count++;
    }

    count = 0;
    cout << endl << "alloc list" << endl;
    for (size_t i = 0; i < alloc_list.size; i++) {
      cout << "       Block " << count
           << " start address: " << (void *)alloc_list.ptr[i].start
           << " Size: " << alloc_list.ptr[i].size << std::endl;
      count++;
    }
    cout << "==========================" << endl;
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
        Block b(tmp, size);
        alloc_list.insert_block(&b);
        heap_size += size;
        return tmp;

      } else if (free_list.ptr[i].size > size) {
        char *tmp = free_list.ptr[i].start;

        // free_list.ptr[i].start = tmp + size;
        // free_list.ptr[i].size = free_list.ptr[i].size - size;
        free_list.update_block(&free_list.ptr[i], tmp + size,
                               free_list.ptr[i].size - size);
        Block b(tmp, size);
        alloc_list.insert_block(&b);
        heap_size += size;
        return tmp;
      }
    }
    // now we have to merge some blocks if there is no readily avaiable block of
    // >=size
    free_list.merge_blocks();

    // then try again to find
    for (size_t i = 0; i < free_list.size; i++) {
      if (free_list.ptr[i].size == size) {
        // removed the block from free list
        char *tmp = free_list.ptr[i].start;
        free_list.remove_block(&free_list.ptr[i]);
        Block b(tmp, size);
        alloc_list.insert_block(&b);
        heap_size += size;
        return tmp;

      } else if (free_list.ptr[i].size > size) {
        char *tmp = free_list.ptr[i].start;

        // free_list.ptr[i].start = tmp + size;
        // free_list.ptr[i].size = free_list.ptr[i].size - size;
        free_list.update_block(&free_list.ptr[i], tmp + size,
                               free_list.ptr[i].size - size);
        Block b(tmp, size);
        alloc_list.insert_block(&b);
        heap_size += size;
        return tmp;
      }
    }
    std::cout << "Algorithm inefficent to resolve the fragmentation";
    return nullptr;
  }
  void Free(void *ptr) {
    // 1. find block in alloc_list
    // 2. clear the memory
    // 3. reomve block from alloc_list
    // 4. add block in the free_list
    Block *block = alloc_list.find_block_by_start_ptr((char *)ptr);
    if (block == nullptr)
      return;

    // memory cleaning
    for (size_t i = 0; i < block->size; i++) {
      block->start[i] = 0;
    }

    free_list.insert_block(block);
    // now we can delete it from alloc_list

    alloc_list.remove_block(block);
    heap_size -= block->size;
  }
  void merge_free_list() { free_list.merge_blocks(); }
};
