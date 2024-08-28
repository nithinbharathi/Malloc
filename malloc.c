#include<stdio.h>
#include<assert.h>
#include<stdlib.h>
#include<stdint.h>
#include<stdbool.h>
#include<string.h>

#define HEAP_CAPACITY 64000
#define LOOKUP_CAPACITY 1024
#define HEAP_CAPACITY_WORDS (HEAP_CAPACITY/sizeof(uintptr_t*))

uintptr_t heap[HEAP_CAPACITY_WORDS] = {0};
size_t heap_size = 0;
uintptr_t* base_address = 0;
bool reachable_blks[1024] = {0};

typedef struct Node Node;
typedef struct mem_blk mem_blk;

struct mem_blk{
  uintptr_t* start;
  size_t size;
};


struct Node{
 char x;
 Node* left;
 Node* right;
};

typedef struct {
  size_t cnt;
  mem_blk mem_blks[LOOKUP_CAPACITY];
}mem_blk_list;

mem_blk_list  allocated_blks = {0};
mem_blk_list free_blks = {
  .cnt = 1,
  .mem_blks = {
	[0] = {.start = heap, .size = sizeof(heap)}
  }
};

mem_blk_list temp  = {0};
Node* root;

void trace_heap(){
  
  printf("Allocated blocks: \n");
  
    for(size_t i = 0;i<allocated_blks.cnt;i++){
	  printf("address: %p\t size: %zu  reachable: %s\n",  allocated_blks.mem_blks[i].start, allocated_blks.mem_blks[i].size, reachable_blks[i]?"true":"false");
	 }
	 
	 printf("\nAvailable free blocks: \n");
	 
	 for(size_t i = 0;i<free_blks.cnt;i++){
	   printf("address: %p\t size: %zu \n", free_blks.mem_blks[i].start, free_blks.mem_blks[i].size);
	 }

	 printf("---------------------------------------------------------------------------------------------------\n");
}

void insert(mem_blk_list* list, void* start_address, size_t size){
  assert(list->cnt<LOOKUP_CAPACITY);

  list->mem_blks[list->cnt].start = start_address;
  list->mem_blks[list->cnt].size = size;

  for(size_t i = list->cnt;i>0 && list->mem_blks[i].start < list->mem_blks[i-1].start; --i){
	const mem_blk blk = list->mem_blks[i];
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

void merge_blks(mem_blk_list* des, const mem_blk_list* src){
  des->cnt = 0;
  
  for(size_t i = 0;i<src->cnt;i++){
	mem_blk current_blk = src->mem_blks[i];
	
	if(des->cnt>0){
	  mem_blk* previous_blk = &des->mem_blks[des->cnt-1];
	  
	  if(previous_blk->start + previous_blk->size == current_blk.start){
		previous_blk->size += current_blk.size;
	  }else{
		insert(des,current_blk.start,current_blk.size);
	  }
	}else{
	  insert(des,current_blk.start,current_blk.size);
	}
  }
}

int comparator(const void* blk1, const void* blk2){
  const mem_blk* b1 = blk1;
  const mem_blk* b2 = blk2;
  
  return b1->start - b2->start;
}
  

int find(const mem_blk_list *list, uintptr_t* start_address){
  for(size_t i = 0;i<list->cnt;i++){
	  if(list->mem_blks[i].start == start_address)
		return i;
   }
	  
   return -1; 
}

void* allocate(size_t size){
  size_t size_in_words = (size + sizeof(uintptr_t) - 1)/sizeof(uintptr_t);
  if( size_in_words <= 0)
	return NULL;
  
  merge_blks(&temp, &free_blks);
  free_blks = temp;

  for(size_t i = 0;i<free_blks.cnt;++i){
	const mem_blk free_blk = free_blks.mem_blks[i];
	
	if(free_blk.size >= size_in_words){
	  remove_blk(&free_blks, i);
	  insert(&allocated_blks, free_blk.start,  size_in_words);
	  
	  const size_t remaining = free_blk.size -  size_in_words;

	  if(remaining>0)
		insert(&free_blks, free_blk.start + size_in_words, remaining);

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


Node* generate_tree(size_t current_level, size_t max_level){
  if(current_level<max_level){
	Node* node = allocate(sizeof(Node));
	
	node->x = (char)('a'+current_level);
	node->left = generate_tree(current_level+1,max_level);
	node->right = generate_tree(current_level+1,max_level);
	
	return node;
  }else{
	
	return NULL;
  }
}

void trace_tree(Node* root){
  if(root != NULL){
	printf("Value of the node: %c at address: %p\n",root->x,root);
	trace_tree(root->left);
	trace_tree(root->right);
  }else{
	return;
  }
}

void mark(const uintptr_t* start, const uintptr_t* end){
  for(; start<end; start+=1){
	const uintptr_t* p = (const uintptr_t*) *start;
	for(size_t i = 0;i<allocated_blks.cnt;i++){
	  mem_blk blk = allocated_blks.mem_blks[i];
	  if(p >= blk.start && p <= blk.start + blk.size){
		if(!reachable_blks[i]){
		  reachable_blks[i] = true;
		  mark(blk.start,blk.start+blk.size);
		}		  
	  }	  
	}
  }   
}

void collect(){
  const uintptr_t* start = __builtin_frame_address(0);

  memset(reachable_blks,0,sizeof(reachable_blks));
		 
  mark(start, base_address+1);

  printf("\nState of the Heap before Deallocation...\n");

  trace_heap();
   
  printf("\nDeallocation started..\n\n");
  
  size_t cnt = 0;
  uintptr_t* to_deallocate[1024];
  
  //collect the blocks to deallocate;
  for(size_t itr = 0; itr<allocated_blks.cnt;itr++){
	if(!reachable_blks[itr]){
	  to_deallocate[cnt++] = allocated_blks.mem_blks[itr].start;
	}
  }
  
  for(size_t itr = 0; itr<cnt;itr++){
	deallocate(to_deallocate[itr]);
  }
}

int main(){
 base_address = __builtin_frame_address(0);
 
 Node* root = generate_tree(0,1);
 
 trace_tree(root);
 printf("The root of the tree is %p\n",root);

  //unreachable pointers
 for(size_t itr = 1 ;itr<=5;itr++){
   allocate(itr); 
 }

 collect();
 trace_heap();

 //allocate to see if the free blocks are getting used;
 allocate(1);
 trace_heap();

 return 1;
}

