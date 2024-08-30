#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "buddy_allocator.h"
#include "bitmap.h"
#include "my_allocator.h"

void print_menu() {
    printf("\nComandi disponibili:\n");
    printf("  alloc [size]  - Alloca 'size' byte di memoria\n");
    printf("  free_all      - Dealloca tutta la memoria allocata\n");
    printf("  exit          - Esci dal programma\n");
    printf("\nInserisci un comando: ");
}

int main() {
    MyAllocator allocator;
    MyAllocator_init(&allocator);

    // array per tenere traccia delle allocazioni
    void* allocations[MEMORY_SIZE / MIN_BUCKET_SIZE];  
    // numero di allocazioni effettuate
    int alloc_count = 0;  
    // buffer per i comandi inseriti dall'utente
    char command[256]; 

    while (true) {
        print_menu();
        fgets(command, sizeof(command), stdin);

        if (strncmp(command, "alloc", 5) == 0) {
            int size;
            if (sscanf(command + 6, "%d", &size) == 1 && size > 0) {
                void* ptr = MyAllocator_malloc(&allocator, size);
                if (ptr) {
                    allocations[alloc_count++] = ptr;
                    if (size < PAGE_SIZE / 4) {
                        printf("Allocazione di %d byte riuscita tramite Buddy Allocator: %p\n", size, ptr);
                        print_bitmap(&allocator.buddy_allocator.bitmap, allocator.buddy_allocator.num_levels);
                    } else {
                        printf("Allocazione di %d byte riuscita tramite mmap: %p\n", size, ptr);
                    }
                } else {
                    printf("Allocazione di %d byte fallita\n", size);
                }
            } else {
                printf("Comando non valido. Specifica una dimensione valida per l'allocazione.\n");
            }
        } else if (strncmp(command, "free_all", 8) == 0) {
            printf("Deallocazione di tutte le allocazioni...\n");
            for (int i = 0; i < alloc_count; i++) {
                MyAllocator_free(&allocator, allocations[i]);
            }
            alloc_count = 0;
            printf("Deallocazione completata.\n");
            print_bitmap(&allocator.buddy_allocator.bitmap, allocator.buddy_allocator.num_levels);
        } else if (strncmp(command, "exit", 4) == 0) {
            printf("Uscita dal programma...\n");
            break;
        } else {
            printf("Comando non riconosciuto. Riprova.\n");
        }
    }

    return 0;
}
