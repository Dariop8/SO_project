#pragma once
#include "bitmap.h"

#define MAX_LEVELS 16 

#define PAGE_SIZE 4096 // 4096 byte (4 KB) dimensione classica di una pagina
#define MAX_BUDDY_SIZE (PAGE_SIZE)
#define MEMORY_SIZE 1048576  // 1 MB = 1048576 byte
#define MIN_BUCKET_SIZE (MEMORY_SIZE>>(MAX_LEVELS))

typedef struct {
    Bitmap bitmap; 
    int num_levels;
    char* memory;           // area di memoria da gestire
    int min_bucket_size;    // dimensione minima del blocco di memoria
    int memory_size;        // dimensione totale della memoria gestita
} BuddyAllocator;

// inizializza il buddy allocator e esegue i vari controlli
void BuddyAllocator_init(BuddyAllocator* allocator, char* memory, int m_size, int min_bucket_size, char* bitmap_memory, int bitmap_memory_size);

// alloca la memoria
void* BuddyAllocator_malloc(BuddyAllocator* alloc, int size);

// rilascia la memoria allocata 
void BuddyAllocator_free(BuddyAllocator* alloc, void* mem);

// funzione ausiliaria che aggiorna il valore dei figli
void Agg_children(Bitmap *bit_map, int bit_num, int status);