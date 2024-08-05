#pragma once
#include "buddy_allocator.h"

#define PAGE_SIZE 4096
#define MEMORY_SIZE (1 * 1024 * 1024) // 1 MB
#define MIN_BUCKET_SIZE 8
#define BUFFER_SIZE 1025
#define MAX_ALLOCS 100 // numero massimo di allocazioni gestite con mmap

// nodo per l'array statico del MyAllocator
typedef struct MmapNode {
    void* mem;
    int size;
    struct MmapNode* next;
} MmapNode;

typedef struct {
    BuddyAllocator buddy_allocator;
    MmapNode mmap_nodes[MAX_ALLOCS];
    int mmap_num; // contatore delle allocazioni con mmap attuali
} MyAllocator;

void MyAllocator_init(MyAllocator* allocator);

void* MyAllocator_malloc(MyAllocator* allocator, int size);

void MyAllocator_free(MyAllocator* allocator, void* mem, int size);
