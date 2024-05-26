#include "buddy_allocator.h"
#include <assert.h>
#include <stdio.h>
#include <math.h>

// livello del nodo idx
int levelIdx(int idx){
  return (int)floor(log2(idx));
};

// indice del buddy del nodo idx
int buddyIdx(int idx){
  if (idx&0x1){
    return idx-1;
  }
  return idx+1;
}

// indice del padre del nodo idx
int parentIdx(int idx){
  return (int)floor(idx/2);
}
 // indice del primo nodo del livello i
int first_in_level(int i){
    return 1<<i;
}

// offset del nodo idx nel suo livello
int offset_in_level(int idx){
    return idx-first_in_level(levelIdx(idx));
}

void BuddyAllocator_init(BuddyAllocator* allocator, char* memory, int m_size, int min_bucket_size, char* bitmap_memory, int bitmap_memory_size) {
    // se m_size non è potenza di 2 trovo la più grande potenza di 2 contenuta in m_size
    int memory_size = m_size;
    if (m_size % 2 != 0) {
        memory_size = 2;
        while (memory_size * 2 < m_size) {
            memory_size *= 2;
        }
    }
    
    // per trovare il numero di livelli simulo gli split fino a raggiungere la dimensione minima
    int num_levels = 0;
    int size = memory_size;
    while (size > min_bucket_size) {
        size /= 2;
        num_levels++;
    }
    allocator->num_levels = num_levels;
    assert(num_levels<=MAX_LEVELS && num_levels>0);
    allocator->memory = memory;
    allocator->memory_size = memory_size;
    allocator->min_bucket_size = min_bucket_size;

    int n_bits = (1 << allocator->num_levels) - 1; // Numero totale di nodi in un albero binario completo 2^h - 1
    assert(bitmap_memory_size >= Bitmap_dim(n_bits));
    Bitmap_init(&allocator->bitmap, bitmap_memory, n_bits);
}

int BuddyAllocator_find_block(BuddyAllocator* allocator, int index, int current_level, int level) {
    // caso base -> sono arrivato al livello desiderato
    if (current_level == level) {
        if (Bitmap_show(&allocator->bitmap, index) == 0) {
            return index;
        } else {
            return -1;
        }
    }

    // caso ricorsivo -> non sono ancora al livello desiderato e il nodo che controllo è libero
    if (Bitmap_show(&allocator->bitmap, index) == 0) {
        int left_child = 2 * index;
        int right_child = 2 * index + 1;
        int found_index = BuddyAllocator_find_block(allocator, left_child, current_level + 1, level);
        if (found_index == -1) {
            found_index = BuddyAllocator_find_block(allocator, right_child, current_level + 1, level);
        }
        return found_index;
    }
    // caso base -> non sono ancora al livello desiderato e il nodo che controllo non è libero
    return -1;
}

void* BuddyAllocator_malloc(BuddyAllocator* allocator, int size) {
    if (size <= 0 || size > allocator->memory_size) {
        printf("!dimensione non valida!");
        return NULL;
    }

    // aggiungo lo spazio necessario a memorizzare l'indice
    size += sizeof(int); 

    // calcolo il livello necessario per il blocco richiesto
    int level = 0;
    int block_size = allocator->memory_size;
    int next_block_size = (allocator->memory_size) / 2;
    while (next_block_size > size) {
        block_size = next_block_size;
        next_block_size /= 2;
        level++;
    }

    // cerco un blocco libero a partire dalla radice
    int index = BuddyAllocator_find_block(allocator, 1, 0, level);
    if (index == -1) {
        printf("!nessun blocco libero di dimensione sufficiente trovato!");
        return NULL;
    }

    // a partire da quello trovato segno i nodi come occupati fino alla radice
    while (index != -1) {
        Bitmap_set(&allocator->bitmap, index, 1);
        index = parentIdx(index);
    }

    // calcolo l'indirizzo del blocco allocato e scrivo ad inzio blocco il suo indice
    int offset = offset_in_level(index);
    void* ris = allocator->memory + offset * block_size; // ricordare disegno delle allocazioni
    *((int *)ris)=index;

    // restituisco il blocco trovato senza mostrare l'indice scritto all'inizio
    return ris + sizeof(int);
}

void BuddyAllocator_free(BuddyAllocator* allocator, void* mem) {
    if (mem==NULL){
        printf("!il blocco non è stato ancora allocato!");
        return;
    }

    // ricavo l'indice
    int index = *((int*)(mem - sizeof(int)));
    
    int dim = allocator->memory_size / (1 << levelIdx(index));
    char *ptr = allocator->memory + offset_in_level(index) * dim;
    if ((int *)ptr != (int*)(mem - sizeof(int))) {
        printf("!il blocco restituito non è allineato!");
        return;
    }
    if (!BitMap_show(&allocator->bitmap, index)) {
        printf("!tentativo di fare una doppia free!");
        return;
    }

    // segno il nodo e i suoi antenati come liberi
    while (index >= 0) {
        Bitmap_set(&allocator->bitmap, index, 0);
        int buddy_index = buddyIdx(index);
        int parent_index = parentIdx(index);

        // se il buddy è libero, lo unisco al nodo corrente
        if (Bitmap_show(&allocator->bitmap, buddy_index) == 0) {
            index = parent_index;
        } else {
            break;
        }
    }
}