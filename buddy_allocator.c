#include "buddy_allocator.h"
#include <assert.h>
#include <stdio.h>
#include <math.h>

// livello del nodo idx
int levelIdx(int idx) {
    return (int)floor(log2(idx + 1));
}

// indice del buddy del nodo idx
int buddyIdx(int idx){
    if (idx == 0) return 0;
    if (idx&0x1){
        return idx+1;
    }
    return idx-1;
}

// indice del padre del nodo idx
int parentIdx(int idx){
    return (int)(idx-1)/2;
}

 // indice del primo nodo del livello i
int first_in_level(int l){
    return (1<<l) -1;
}

// offset del nodo idx nel suo livello
int offset_in_level(int idx){
    int l = levelIdx(idx);
    return idx - first_in_level(l);
}

void BuddyAllocator_init(BuddyAllocator* allocator, char* memory, int m_size, int min_bucket_size, char* bitmap_memory, int bitmap_memory_size) {
    assert(allocator != NULL);
    assert(memory != NULL);
    assert(bitmap_memory != NULL);

    // se m_size non è potenza di 2 trovo la più grande potenza di 2 contenuta in m_size
    int memory_size = 1;
    while (memory_size <= m_size) {
        memory_size <<= 1;
    }
    if (memory_size > m_size) {
        memory_size >>= 1;
    }
    allocator->memory_size = memory_size;

    // se min_bucket_size non è potenza di 2 trovo la più grande potenza di 2 contenuta in min_bucket_size
    int min_size = 1;
    while (min_size <= min_bucket_size) {
        min_size <<= 1;
    }
    if (min_size > min_bucket_size) {
        min_size >>= 1;
    }
    allocator->min_bucket_size = min_size;

    // per trovare il numero di livelli simulo gli split fino a raggiungere la dimensione minima
    int num_levels = 0;
    int size = memory_size;
    while (size >= min_size) {
        size /= 2;
        num_levels++;
    }
    allocator->num_levels = num_levels;
    assert(num_levels<=MAX_LEVELS && num_levels>0);

    allocator->memory = memory;

    int n_bits = (1 << num_levels) - 1; // Numero totale di nodi in un albero binario completo 2^h - 1
    assert(bitmap_memory_size >= Bitmap_dim(n_bits));
    Bitmap_init(&allocator->bitmap, bitmap_memory, n_bits);
}

void* BuddyAllocator_malloc(BuddyAllocator* allocator, int size) {
    if (size <= 0 || size > allocator->memory_size) {
        printf("!dimensione non valida!");
        return NULL;
    }

    // aggiungo lo spazio necessario a memorizzare l'indice
    size += sizeof(int); 

    // calcolo il livello necessario per il blocco richiesto
    int level = allocator->num_levels - 1;
    int block_size = allocator->min_bucket_size;
    while (block_size < size) {
        block_size <<= 1;
        level--;
    }

    // cerco un blocco libero a partire dal livello calcolato
    int index = first_in_level(level);
    while (index < first_in_level(level + 1)) {
        if (Bitmap_show(&allocator->bitmap, index) == 0) {
            // trovato, aggiorno la bitmap
            Bitmap_set(&allocator->bitmap, index, 1);
            Agg_children(&allocator->bitmap, index, 1);

            // propago l'aggiornamento ai nodi genitori
            int parent = parentIdx(index);
            while (parent >= 0) {
                Bitmap_set(&allocator->bitmap, parent, 1);
                parent = parentIdx(parent);
            }

            // memorizzo l'indice del blocco all'inizio del blocco di memoria
            int* block_ptr = (int*)(allocator->memory + (index - first_in_level(level)) * block_size);
            *block_ptr = index;

            // restituisco il puntatore alla memoria, subito dopo l'indice
            return (void*)(block_ptr + 1);
        }
        index++;
    }

    // nessun blocco disponibile
    return NULL; 
}

void BuddyAllocator_free(BuddyAllocator* allocator, void* mem) {
    if (mem==NULL){
        printf("!il blocco non è stato ancora allocato!");
        return;
    }
    if ((char*)mem < allocator->memory || (char*)mem >= allocator->memory + allocator->memory_size) {
        printf("!il blocco non è nei margini!");
        return;
    }


    // Recupera l'indice del blocco dalla memoria
    int* block_ptr = (int*)mem - 1;
    int index = *block_ptr;

    int level = levelIdx(index);
    int dim = allocator->memory_size / (1 << level);
    char *ptr = allocator->memory + offset_in_level(index) * dim;
    if ((void *)ptr != (void*)(mem - sizeof(int))) {
        printf("!Il blocco restituito non è allineato!\n");
        return;
    }
    if (!Bitmap_show(&allocator->bitmap, index)) {
        printf("!Tentativo di fare una doppia free!\n");
        return;
    }

    // segno il nodo e i suoi figli come liberi nella bitmap
    Bitmap_set(&allocator->bitmap, index, 0);
    Agg_children(&allocator->bitmap, index, 0);

    // faccio il merge dei buddy se possibile
    int buddy = buddyIdx(index);
    while (index > 0 && Bitmap_show(&allocator->bitmap, buddy) == 0) {
        index = parentIdx(index);
        Bitmap_set(&allocator->bitmap, index, 0);
        buddy = buddyIdx(index);
    }
}

void Agg_children(Bitmap *bit_map, int bit_num, int status){
  if (bit_num < bit_map->n_bits){
    Bitmap_set(bit_map, bit_num, status);
    Agg_children(bit_map, 2 * bit_num + 1, status);  //figlio sinistro
    Agg_children(bit_map, 2 * bit_num + 2, status);  //figlio destro
  }
}