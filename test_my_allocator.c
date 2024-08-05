#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include "buddy_allocator.h"
#include "bitmap.h"
#include "my_allocator.h"

int main() {
    MyAllocator allocator;
    MyAllocator_init(&allocator);

    printf("Bitmap iniziale:\n");
    print_bitmap(&allocator.buddy_allocator.bitmap, allocator.buddy_allocator.num_levels);

    // Test: Allocazione valida di una piccola dimensione
    printf("Test 1: Allocazione valida di una piccola dimensione\n");
    void* ptr1 = MyAllocator_malloc(&allocator, MIN_BUCKET_SIZE / 2);
    if (ptr1) {
        printf("Allocazione riuscita: %p\n", ptr1);
        print_bitmap(&allocator.buddy_allocator.bitmap, allocator.buddy_allocator.num_levels);
    } else {
        printf("Allocazione fallita\n");
    }

    // Test: Allocazione valida di una grande dimensione
    printf("Test 2: Allocazione valida di una grande dimensione\n");
    void* ptr2 = MyAllocator_malloc(&allocator, PAGE_SIZE);
    if (ptr2) {
        printf("Allocazione riuscita: %p\n", ptr2);
    } else {
        printf("Allocazione fallita\n");
    }

    // Test: Rilascio di un'allocazione valida piccola
    printf("Test 5: Rilascio di un'allocazione valida piccola\n");
    MyAllocator_free(&allocator, ptr1, MIN_BUCKET_SIZE / 2);
    printf("Rilascio riuscito di ptr1\n");
    print_bitmap(&allocator.buddy_allocator.bitmap, allocator.buddy_allocator.num_levels);

    // Test: Rilascio di un'allocazione valida grande
    printf("Test 6: Rilascio di un'allocazione valida grande\n");
    MyAllocator_free(&allocator, ptr2, PAGE_SIZE);
    printf("Rilascio riuscito di ptr2\n");

    // Test: Allocazione invalida di una dimensione negativa
    printf("Test 3: Allocazione invalida di una dimensione negativa\n");
    void* ptr3 = MyAllocator_malloc(&allocator, -100);
    if (ptr3) {
        printf("Allocazione riuscita: %p (non dovrebbe succedere)\n", ptr3);
    } else {
        printf("Allocazione fallita (atteso)\n");
    }

    // Test: Rilascio invalido di un puntatore nullo
    printf("Test 7: Rilascio invalido di un puntatore nullo\n");
    MyAllocator_free(&allocator, NULL, 0);
    printf("Rilascio di un puntatore nullo (atteso)\n");

    // Test: Rilascio invalido di un puntatore non allocato
    printf("Test 8: Rilascio invalido di un puntatore non allocato\n");
    int dummy;
    MyAllocator_free(&allocator, &dummy, sizeof(dummy));
    printf("Rilascio di un puntatore non allocato (atteso)\n");

    // Test: Allocazione valida ripetuta per riempire il buddy allocator
    printf("Test 9: Allocazione valida ripetuta per riempire il buddy allocator\n");
    void* allocations[MEMORY_SIZE / MIN_BUCKET_SIZE];
    int success_count = 0;
    for (int i = 0; i < (MEMORY_SIZE / MIN_BUCKET_SIZE); i++) {
        allocations[i] = MyAllocator_malloc(&allocator, MIN_BUCKET_SIZE / 2);
        if (allocations[i]) {
            success_count++;
        } else {
            break;
        }
    }
    printf("Allocazioni riuscite: %d\n", success_count);
    print_bitmap(&allocator.buddy_allocator.bitmap, allocator.buddy_allocator.num_levels);

    // Test: Rilascio di tutte le allocazioni
    printf("Test 10: Rilascio di tutte le allocazioni\n");
    for (int i = 0; i < success_count; i++) {
        MyAllocator_free(&allocator, allocations[i], MIN_BUCKET_SIZE / 2);
    }
    printf("Rilascio riuscito di tutte le allocazioni\n");
    print_bitmap(&allocator.buddy_allocator.bitmap, allocator.buddy_allocator.num_levels);

    return 0;
}