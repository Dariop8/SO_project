#include "buddy_allocator.h"
#include <assert.h>

void BuddyAllocator_init(BuddyAllocator* allocator, char* memory, int m_size, int min_bucket_size, char* bitmap_memory, int bitmap_memory_size) {
    // trovo la potenza di 2 successiva che è maggiore o uguale a m_size
    int memory_size = 1;
    while (memory_size < m_size) {
        memory_size *= 2;
    }
    
    // per trovare il numero di livelli simulo gli split fino a raggiungere la dimensione minima
    int num_levels = 0;
    int size = memory_size;
    while (size > min_bucket_size) {
        size /= 2;
        num_levels++;
    }
    allocator->num_levels = num_levels + 1;
    assert(num_levels<=MAX_LEVELS);
    allocator->memory = memory;
    allocator->memory_size = memory_size;
    allocator->min_bucket_size = min_bucket_size;

    int n_bits = (1 << allocator->num_levels) - 1; // Numero totale di nodi in un albero binario completo 2^h - 1
    assert(bitmap_memory_size >= Bitmap_dim(n_bits));
    Bitmap_init(&allocator->bitmap, bitmap_memory, n_bits);

    // segno l'intera memoria come libera, indicando che la radice dell'albero è libera
    Bitmap_set(&allocator->bitmap, 0, 1);
}
