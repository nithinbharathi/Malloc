#include<stdio.h>
#define CAPACITY 64000

char heap[CAPACITY] = {0};
size_t heap_size = 0;

typedef struct heap_alloc_info{
       void* start;
       size_t size;
       
}heap_info;

heap_info heap_info_lookup[1024];
int heap_info_size;

void* allocate(size_t size){
    void *result = heap + heap_size;
    heap_size+=size;

    heap_info obj;
    obj.start = result;
    obj.size = size;

    heap_info_lookup[heap_info_size++] = obj;
    return result;
}

void print_heap_info(){
     for(int i = 0;i<heap_info_size;i++){
     printf("address: %p\t size: %zu \n",heap_info_lookup[i].start,heap_info_lookup[i].size);
}
}
void main(){

    for(int i = 0;i<26;i++){
	allocate(i);
    }

    print_heap_info();
    
}
