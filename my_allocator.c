#include "my_allocator.h"
#include <assert.h>
#include <stdio.h>
#include <sys/mman.h>

void MyAllocator_init(MyAllocator* allocator) {
    static char buddy_memory[MEMORY_SIZE];
    static char bitmap_memory[BUFFER_SIZE];

    BuddyAllocator_init(&allocator->buddy_allocator, buddy_memory, MEMORY_SIZE, MIN_BUCKET_SIZE, bitmap_memory, BUFFER_SIZE);
    
    for (int i = 0; i < MAX_ALLOCS; i++) {
        allocator->mmap_nodes[i].mem = NULL;
        allocator->mmap_nodes[i].size = 0;
    }

    allocator->mmap_num = 0;
}

void* MyAllocator_malloc(MyAllocator* allocator, int size) {
    if (size < PAGE_SIZE / 4) {
        // bisogna usare il buddy allocator
        return BuddyAllocator_malloc(&allocator->buddy_allocator, size);
    } else {
        // bisogna usare mmap
        void* mem = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE, -1, 0); // MAP_ANON perché l'mmap non è associata a nessun fd, MAP_PRIVATE per renderla privata per il processo chiamante 
        if (mem == MAP_FAILED) {
            printf("!errore con mmap!\n");
            return NULL;
        }

        // aggiungo l'allocazione all'array mmap_nodes
        for (int i = 0; i < MAX_ALLOCS; i++) {
            if (allocator->mmap_nodes[i].mem == NULL) {
                allocator->mmap_nodes[i].mem = mem;
                allocator->mmap_nodes[i].size = size;
                allocator->mmap_num++;
                return mem;
            }
        }

        // già raggiunto il numero massimo di allocazioni con mmmap
        munmap(mem, size);
        printf("!allocazione non avvenuta, numero massimo raggiunto!\n");
        return NULL;
    }
}

void MyAllocator_free(MyAllocator* allocator, void* mem) {
    if (mem == NULL) return;

    char* buddy_memory_start = allocator->buddy_allocator.memory;
    char* buddy_memory_end = buddy_memory_start + allocator->buddy_allocator.memory_size;

    if (mem >= (void*)buddy_memory_start && mem < (void*)buddy_memory_end) {
        // il puntatore si trova nell'area del buddy allocator
        BuddyAllocator_free(&allocator->buddy_allocator, mem);
    } else {
        // il puntatore non si trova nell'area del buddy allocator, quindi deve essere gestito con munmap
        for (int i = 0; i < MAX_ALLOCS; i++) {
            if (allocator->mmap_nodes[i].mem == mem) {
                munmap(mem, allocator->mmap_nodes[i].size);
                allocator->mmap_nodes[i].mem = NULL;
                allocator->mmap_nodes[i].size = 0;
                allocator->mmap_num--;
                return;
            }
        }
    }
}

