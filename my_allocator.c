#include "my_allocator.h"
#include <assert.h>
#include <stdio.h>
#include <sys/mman.h>

void MyAllocator_init(MyAllocator* allocator) {
    static char buddy_memory[MEMORY_SIZE];
    static char bitmap_memory[BUFFER_SIZE];

    BuddyAllocator_init(&allocator->buddy_allocator, buddy_memory, MEMORY_SIZE, MIN_BUCKET_SIZE, bitmap_memory, BUFFER_SIZE);
    
    allocator->mmap_list = NULL;
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

        // aggiungo l'allocazione alla linked list
        MmapNode* new_node = (MmapNode*)mmap(NULL, sizeof(MmapNode), PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE, -1, 0);
        if (new_node == MAP_FAILED) {
            printf("!errore con mmap per il nodo della linked list!\n");
            munmap(mem, size);
            return NULL;
        }

        new_node->mem = mem;
        new_node->size = size;
        new_node->next = allocator->mmap_list;
        allocator->mmap_list = new_node;

        return mem;
    }
}

void MyAllocator_free(MyAllocator* allocator, void* mem, size_t size) {
    if (size < PAGE_SIZE / 4) {
        // bisogna usare il buddy allocator
        BuddyAllocator_free(&allocator->buddy_allocator, mem);
    } else {
        // bisogna usare munmap
        MmapNode** node = &allocator->mmap_list;
        while (*node) {
            if ((*node)->mem == mem) {
                MmapNode* node_app = *node;
                *node = (*node)->next;
                munmap(mem, node_app->size);
                munmap(node_app, sizeof(MmapNode));
                return;
            }
            node = &(*node)->next;
        }
    }
}