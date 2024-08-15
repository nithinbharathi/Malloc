#include<stdio.h>
#include<assert.h>
#include<stdlib.h>

#define HEAP_CAPACITY 64000
#define LOOKUP_CAPACITY 1024

char heap[HEAP_CAPACITY] = {0};
size_t heap_size = 0;

typedef struct memory_blk{
       char* start;
       size_t size;
}memory_blk;

typedef struct {
  size_t cnt;
  memory_blk mem_blks[LOOKUP_CAPACITY];
}mem_blk_list;

mem_blk_list  allocated_blks;
mem_blk_list free_blks;

int lookup_table_size = 0,free_lookup_table_size = 0;

void insert(mem_blk_list* list, void* start_address, size_t size){
  assert(list->cnt<LOOKUP_CAPACITY);

  list->mem_blks[list->cnt].start = start_address;
  list->mem_blks[list->cnt].size = size;

  for(size_t i = list->cnt;i>0 && list->mem_blks[i].start < list->mem_blks[i-1].start; --i){
	const memory_blk blk = list->mem_blks[i];
	list->mem_blks[i] = list->mem_blks[i-1];
	list->mem_blks[i-1] = blk;
  }
  
  list->cnt++;
}

void* allocate(size_t size){
  if(size <= 0)
	return NULL;
  
  assert(heap_size + size <= HEAP_CAPACITY);
  
  void *address = heap + heap_size;
  heap_size += size;

  insert(&allocated_blks, address, size);
  
  return address;
}

int comparator(const void* blk1, const void* blk2){
  const memory_blk *b1 = blk1;
  const memory_blk *b2 = blk2;
  
  return b1->start - b2->start;
}
  

int find(mem_blk_list *allocated_blks, void* start_address){
  memory_blk blk = {
	.start = start_address
  };
  
   memory_blk* found_address = bsearch(&blk, allocated_blks->mem_blks, allocated_blks->cnt, sizeof(allocated_blks->mem_blks[0]),comparator);

   if(found_address != 0)
	 return ((found_address - allocated_blks->mem_blks)/sizeof(allocated_blks->mem_blks[0]));
   else
	 return -1;
  
}

void deallocate(void* blk_address){
  if(blk_address)
	return;
  
  const int index  = find(&allocated_blks, blk_address);
  assert(index>=0);

  insert(&free_blks,allocated_blks.mem_blks[index].start, allocated_blks.mem_blks[index].size);
  
}

void trace_heap(const mem_blk_list* list){
     for(size_t i = 0;i<list->cnt;i++){
	   printf("address: %p\t size: %zu \n", list->mem_blks[i].start, list->mem_blks[i].size);
	 }
}

int main(){
    for(int i = 0;i<26;i++){
	  allocate(i);
    }

    trace_heap(&allocated_blks);
	return 1;
}
