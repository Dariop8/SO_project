#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "buddy_allocator.h"
#include "bitmap.h"

#define MEMORY_SIZE 1024  
#define MIN_BUCKET_SIZE 32  

void print_bitmap(Bitmap* bitmap, int num_levels) {
    int index = 0;
    for (int level = 0; level < num_levels; level++) {
        int nodes = 1 << level;
        for (int node = 0; node < nodes; node++) {
            printf("%d ", Bitmap_show(bitmap, index));
            index++;
        }
        printf("\n");
    }
}

int main() {
    char memory[MEMORY_SIZE];
    int bitmap_size = Bitmap_dim(MEMORY_SIZE / MIN_BUCKET_SIZE);
    char bitmap_memory[bitmap_size];
    BuddyAllocator allocator;

    //BuddyAllocator_init(&allocator, memory, MEMORY_SIZE, MIN_BUCKET_SIZE, bitmap_memory);

    printf("Bitmap iniziale 1024 alla radice:\n");
    print_bitmap(&allocator.bitmap, allocator.num_levels);

    // provo ad allocare alcuni blocchi di memoria
    void* block1 = BuddyAllocator_malloc(&allocator, 64);
    void* block2 = BuddyAllocator_malloc(&allocator, 128);
    void* block3 = BuddyAllocator_malloc(&allocator, 32);

    // stampo le allocazioni per verificare il funzionamento
    printf("Allocated block1 at: %p\n", block1);
    printf("Allocated block2 at: %p\n", block2);
    printf("Allocated block3 at: %p\n", block3);

    printf("Bitmap dopo le allocazioni di 64, 128, 32:\n");
    print_bitmap(&allocator.bitmap, allocator.num_levels);

    // provo a liberare alcuni blocchi di memoria
    BuddyAllocator_free(&allocator, block2);
    BuddyAllocator_free(&allocator, block1);

    printf("Bitmap dopo le free di 64, 128:\n");
    print_bitmap(&allocator.bitmap, allocator.num_levels);

    // faccio un'altra allocazione
    void* block4 = BuddyAllocator_malloc(&allocator, 64);
    printf("Allocated block4 at: %p (after freeing block1 and block2)\n", block4);

    printf("Bitmap dopo l'allocazione di 64:\n");
    print_bitmap(&allocator.bitmap, allocator.num_levels);

    // libero i blocchi rimanenti
    BuddyAllocator_free(&allocator, block3);
    BuddyAllocator_free(&allocator, block4);

    printf("Bitmap alla fine dopo aver liberato tutto:\n");
    print_bitmap(&allocator.bitmap, allocator.num_levels);

    return 0;
}