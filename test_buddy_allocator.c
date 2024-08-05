#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include "buddy_allocator.h"
#include "bitmap.h"

#define BUFFER_SIZE 1025
#define BUDDY_LEVELS 5
#define MEMORY_SIZE 256
#define MIN_BUCKET_SIZE 8

int main() {
    // memoria principale e bitmap
    char memory[MEMORY_SIZE];
    char bitmap_memory[BUFFER_SIZE];

    BuddyAllocator allocator;
    BuddyAllocator_init(&allocator, memory, MEMORY_SIZE, MIN_BUCKET_SIZE, bitmap_memory, BUFFER_SIZE);

    printf("Bitmap iniziale:\n");
    print_bitmap(&allocator.bitmap, allocator.num_levels);

    // allocazione di 16 byte
    void* block1 = BuddyAllocator_malloc(&allocator, 16);
    printf("\nBitmap dopo allocazione di 16 byte:\n");
    print_bitmap(&allocator.bitmap, allocator.num_levels);

    // allocazione di 32 byte
    void* block2 = BuddyAllocator_malloc(&allocator, 32);
    printf("\nBitmap dopo allocazione di 32 byte:\n");
    print_bitmap(&allocator.bitmap, allocator.num_levels);

    // allocazione di 64 byte
    void* block3 = BuddyAllocator_malloc(&allocator, 64);
    printf("\nBitmap dopo allocazione di 64 byte:\n");
    print_bitmap(&allocator.bitmap, allocator.num_levels);

    // allocazione di 128 byte (dovrebbe fallire)
    void* block4 = BuddyAllocator_malloc(&allocator, 128);
    if (block4 == NULL) {
        printf("\nAllocazione di 128 byte fallita come previsto\n");
    } else {
        printf("\nBitmap dopo allocazione di 128 byte:\n");
        print_bitmap(&allocator.bitmap, allocator.num_levels);
    }

    // deallocazioni del blocco da 16 byte
    BuddyAllocator_free(&allocator, block1);
    printf("\nBitmap dopo deallocazione del blocco da 16 byte:\n");
    print_bitmap(&allocator.bitmap, allocator.num_levels);

    // deallocazione del blocco da 32 byte
    BuddyAllocator_free(&allocator, block2);
    printf("\nBitmap dopo deallocazione del blocco da 32 byte:\n");
    print_bitmap(&allocator.bitmap, allocator.num_levels);

    // allocazione di 8 byte
    void* block5 = BuddyAllocator_malloc(&allocator, 8);
    printf("\nBitmap dopo allocazione di 8 byte:\n");
    print_bitmap(&allocator.bitmap, allocator.num_levels);

    // deallocazione del blocco da 64 byte
    BuddyAllocator_free(&allocator, block3);
    printf("\nBitmap dopo deallocazione del blocco da 64 byte:\n");
    print_bitmap(&allocator.bitmap, allocator.num_levels);

    // deallocazione del blocco da 8 byte
    BuddyAllocator_free(&allocator, block5);
    printf("\nBitmap dopo deallocazione del blocco da 8 byte:\n");
    print_bitmap(&allocator.bitmap, allocator.num_levels);

    // allocazione e deallocazione consecutiva di blocchi da 8 byte
    void* blocks[16];
    for (int i = 0; i < 16; i++) {
        blocks[i] = BuddyAllocator_malloc(&allocator, 8);
    }
    printf("\nBitmap dopo allocazione di 16 blocchi da 8 byte:\n");
    print_bitmap(&allocator.bitmap, allocator.num_levels);

    for (int i = 0; i < 16; i++) {
        BuddyAllocator_free(&allocator, blocks[i]);
    }
    printf("\nBitmap dopo deallocazione di 16 blocchi da 8 byte:\n");
    print_bitmap(&allocator.bitmap, allocator.num_levels);

    // deallocazione del blocco da 64 byte non allocato (dovrebbe fallire)
    BuddyAllocator_free(&allocator, block3);
    printf("\nBitmap dopo deallocazione del blocco da 64 byte non allocato:\n");
    print_bitmap(&allocator.bitmap, allocator.num_levels);

    return 0;
}