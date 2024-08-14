#include<stdio.h>
#include<assert.h>

#define HEAP_CAPACITY 64000

char heap[HEAP_CAPACITY] = {0};
size_t heap_size = 0;

typedef struct memory_blk{
       void* start;
       size_t size;
       
}memory_blk;

memory_blk  memory_blk_lookup[1024]; 
int lookup_table_size = 0;

void* allocate(size_t size){
   assert(heap_size +size <= HEAP_CAPACITY);
  
    void *address = heap + heap_size;
    heap_size+=size;

    memory_blk  obj;
    obj.start = address;
    obj.size = size;

    memory_blk_lookup[lookup_table_size++] = obj;
	
    return address;
}

void* deallocate(void* blk){
  
}

void print_heap_info(){
     for(int i = 0;i<lookup_table_size;i++){
     printf("address: %p\t size: %zu \n", memory_blk_lookup[i].start, memory_blk_lookup[i].size);
	 }
}

void main(){

    for(int i = 0;i<26;i++){
	allocate(i);
    }

    print_heap_info();
}
