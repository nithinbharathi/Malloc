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

mem_blk_list  allocated_blks = {0};
mem_blk_list free_blks = {
  .cnt = 1,
  .mem_blks = {
	[0] = {.start = heap, .size = sizeof(heap)}
  }
};

int lookup_table_size = 0,free_lookup_table_size = 0;

void trace_heap(const mem_blk_list* list){
     for(size_t i = 0;i<list->cnt;i++){
	   printf("address: %p\t size: %zu \n", list->mem_blks[i].start, list->mem_blks[i].size);
	 }
}

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

void remove_blk(mem_blk_list* list, size_t index){
  assert(index < list->cnt);
  
  for(size_t i = index;i<list->cnt-1;++i){
	list->mem_blks[i] = list->mem_blks[i+1];
  }
  
  list->cnt--;
}

int comparator(const void* blk1, const void* blk2){
  const memory_blk *b1 = blk1;
  const memory_blk *b2 = blk2;
  
  return b1->start - b2->start;
}
  

int find(const mem_blk_list *list, void* start_address){
  memory_blk blk = {
	.start = start_address
  };
  
  for(size_t i = 0;i<list->cnt;i++){
	  if(list->mem_blks[i].start == start_address)
		return i;
   }
	  
   return -1; 
}

void* allocate(size_t size){
  if(size <= 0)
	return NULL;
  
  for(size_t i = 0;i<free_blks.cnt;++i){
	const memory_blk free_blk = free_blks.mem_blks[i];
	
	if(free_blk.size>=size){
	  remove_blk(&free_blks, i);
	  insert(&allocated_blks, free_blk.start, size);
	  
	  const size_t remaining = free_blk.size - size;

	  if(remaining>0)
		insert(&free_blks,free_blk.start+size,remaining);

	  return free_blk.start;
	}
  }
 
  return NULL;
}

void deallocate(void* blk_address){
  if(blk_address == NULL)
	return;
  
  const int index  = find(&allocated_blks, blk_address);
  assert(index>=0);
  
  insert(&free_blks,allocated_blks.mem_blks[index].start, allocated_blks.mem_blks[index].size);
  remove_blk(&allocated_blks, (size_t)index);
  
}

int main(){
  for(int i = 0;i<8;i++){
	void* allocated_address = allocate(i);
	if(i%2 == 0)
	  deallocate(allocated_address);
  }
    trace_heap(&allocated_blks);
	printf("free blocks...\n");
	trace_heap(&free_blks);

	for(int i = 0;i<8;i++){
	  if(i%2==0){
		allocate(i);
	  }
	}
	
	trace_heap(&allocated_blks);
	printf("free blocks after allocation...\n");
	trace_heap(&free_blks);

	return 1;
}
