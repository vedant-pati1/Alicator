This is a header only custom memory allocation C++ library which work in OS-less environments. It provides a simple API for memory allocation and deallocation.

usage example:

```cpp

#include "heap.hpp"

int main() {
  char mem[20];

  Block_array b_arr;
  Block arr[20];

  b_arr.ptr = arr;
  b_arr.capacity = 20;

  Heap h(mem, 20, b_arr);

// your code here
  return 0;
}
```


