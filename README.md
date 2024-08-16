# Malloc

This project provides a simple memory management simulation in C. It demonstrates basic memory allocation and deallocation mechanisms using a fixed-size heap. The implementation also includes an additional functionality for tracking and merging memory blocks to optimize space utilization. The key features include memory allocation, memory deallocation, and merging contiguous free blocks to optimize memroy usage.

The following example demonstrates allocation and deallocation of memory blocks, and prints the state of allocated and free memory blocks:

'''
for (int i = 0; i < 5; i++) {             //allocates memory blocks of size 0 to 4
    void* allocated_address = allocate(i);
    deallocate(allocated_address);        //deallocates the allocated memory addresses
  }
  
trace_heap(&allocated_blks);              // prints out the current state of the allocated blocks
trace_heap(&free_blks);                   //prints out the current state of all the free blocks available
'''
