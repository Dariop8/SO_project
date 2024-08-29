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

    // Test 1: Allocazione valida di una piccola dimensione
    printf("Test 1: Allocazione valida di una piccola dimensione\n");
    void* ptr1 = MyAllocator_malloc(&allocator, MIN_BUCKET_SIZE / 2);
    if (ptr1) {
        printf("Allocazione riuscita: %p\n", ptr1);
        print_bitmap(&allocator.buddy_allocator.bitmap, allocator.buddy_allocator.num_levels);
    } else {
        printf("Allocazione fallita\n");
    }

    // Test 2: Allocazione valida di una grande dimensione
    printf("Test 2: Allocazione valida di una grande dimensione\n");
    void* ptr2 = MyAllocator_malloc(&allocator, PAGE_SIZE);
    if (ptr2) {
        printf("Allocazione riuscita: %p\n", ptr2);
        // Deallocazione immediata
        printf("Deallocazione immediata di ptr2: %p\n", ptr2);
        MyAllocator_free(&allocator, ptr2, PAGE_SIZE);
    } else {
        printf("Allocazione fallita\n");
    }

    // Test 3: Allocazione invalida di una dimensione negativa
    printf("Test 3: Allocazione invalida di una dimensione negativa\n");
    void* ptr3 = MyAllocator_malloc(&allocator, -100);
    if (ptr3) {
        printf("Allocazione riuscita: %p (non dovrebbe succedere)\n", ptr3);
        // Deallocazione immediata
        printf("Deallocazione immediata di ptr3: %p\n", ptr3);
        MyAllocator_free(&allocator, ptr3, -100);
    } else {
        printf("Allocazione fallita (atteso)\n");
    }

    // Test 4: Rilascio di un'allocazione valida piccola
    printf("Test 4: Rilascio di un'allocazione valida piccola\n");
    if (ptr1) {
        printf("Deallocazione di ptr1: %p\n", ptr1);
        MyAllocator_free(&allocator, ptr1, MIN_BUCKET_SIZE / 2);
        print_bitmap(&allocator.buddy_allocator.bitmap, allocator.buddy_allocator.num_levels);
    }

    // Test 5: Rilascio invalido di un puntatore nullo
    printf("Test 5: Rilascio invalido di un puntatore nullo\n");
    MyAllocator_free(&allocator, NULL, 0);
    printf("Rilascio di un puntatore nullo (atteso)\n");

    // Test 6: Rilascio invalido di un puntatore non allocato
    printf("Test 6: Rilascio invalido di un puntatore non allocato\n");
    int dummy;
    MyAllocator_free(&allocator, &dummy, sizeof(dummy));
    printf("Rilascio di un puntatore non allocato (atteso)\n");

    // Test 7: Allocazione valida ripetuta per riempire il buddy allocator
    printf("Test 7: Allocazione valida ripetuta per riempire il buddy allocator\n");
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

    // Test 8: Allocazione di una piccola dimensione con buddy allocator pieno
    printf("Test 8: Allocazione di una piccola dimensione con buddy allocator pieno\n");
    void* ptr4 = MyAllocator_malloc(&allocator, MIN_BUCKET_SIZE / 2);
    if (ptr4) {
        printf("Allocazione riuscita: %p\n", ptr4);
        print_bitmap(&allocator.buddy_allocator.bitmap, allocator.buddy_allocator.num_levels);
    } else {
        printf("Allocazione fallita (atteso)\n");
    }

    // Deallocazione di tutte le allocazioni fatte nel buddy allocator
    printf("Deallocazione di tutte le allocazioni fatte nel buddy allocator\n");
    for (int i = 0; i < success_count; i++) {
        MyAllocator_free(&allocator, allocations[i], MIN_BUCKET_SIZE / 2);
    }
    printf("Deallocazione completata delle allocazioni nel buddy allocator\n");

    // Test 9: Allocazione valida di una piccola dimensione (minima possibile)
    printf("Test 9: Allocazione valida di una piccola dimensione (minima possibile)\n");
    void* ptr5 = MyAllocator_malloc(&allocator, MIN_BUCKET_SIZE / 4);
    if (ptr5) {
        printf("Allocazione riuscita: %p\n", ptr5);
        print_bitmap(&allocator.buddy_allocator.bitmap, allocator.buddy_allocator.num_levels);
    } else {
        printf("Allocazione fallita\n");
    }

    // Test 10: Allocazione valida di una grande dimensione (esattamente 1/4 della dimensione della pagina)
    printf("Test 10: Allocazione valida di una grande dimensione (esattamente 1/4 della dimensione della pagina)\n");
    void* ptr6 = MyAllocator_malloc(&allocator, PAGE_SIZE / 4);
    if (ptr6) {
        printf("Allocazione riuscita: %p\n", ptr6);
        // Deallocazione immediata
        printf("Deallocazione immediata di ptr6: %p\n", ptr6);
        MyAllocator_free(&allocator, ptr6, PAGE_SIZE / 4);
    } else {
        printf("Allocazione fallita\n");
    }

    // Test 11: Allocazione esatta di una dimensione uguale alla dimensione di una pagina (usa mmap)
    printf("Test 11: Allocazione esatta di una dimensione uguale alla dimensione di una pagina (usa mmap)\n");
    void* ptr7 = MyAllocator_malloc(&allocator, PAGE_SIZE);
    if (ptr7) {
        printf("Allocazione riuscita: %p\n", ptr7);
        // Deallocazione immediata
        printf("Deallocazione immediata di ptr7: %p\n", ptr7);
        MyAllocator_free(&allocator, ptr7, PAGE_SIZE);
    } else {
        printf("Allocazione fallita\n");
    }

    // Test 12: Allocazione di una dimensione leggermente inferiore a 1/4 della dimensione di una pagina
    printf("Test 12: Allocazione di una dimensione leggermente inferiore a 1/4 della dimensione di una pagina\n");
    void* ptr8 = MyAllocator_malloc(&allocator, (PAGE_SIZE / 4) - 1);
    if (ptr8) {
        printf("Allocazione riuscita: %p\n", ptr8);
        print_bitmap(&allocator.buddy_allocator.bitmap, allocator.buddy_allocator.num_levels);
    } else {
        printf("Allocazione fallita\n");
    }

    // Test 13: Allocazione grande ripetuta (diverse mmap consecutive)
    printf("Test 13: Allocazione grande ripetuta (diverse mmap consecutive)\n");
    void* mmap_allocations[MAX_ALLOCS];
    int mmap_count = 0;
    for (int i = 0; i < MAX_ALLOCS; i++) {
        mmap_allocations[i] = MyAllocator_malloc(&allocator, PAGE_SIZE * (i + 1));
        if (mmap_allocations[i]) {
            printf("Allocazione mmap %d riuscita: %p\n", i + 1, mmap_allocations[i]);
            mmap_count++;
        } else {
            printf("Allocazione mmap %d fallita\n", i + 1);
            break;
        }
    }

    // Test 14: Allocazione grande oltre il numero massimo
    printf("Test 14: Allocazione grande oltre il numero massimo\n");
    void* ptr11 = MyAllocator_malloc(&allocator, PAGE_SIZE);
    if (ptr11) {
        printf("Allocazione riuscita: %p\n (inatteso)", ptr11);
    } else {
        printf("Allocazione fallita (atteso)\n");
    }

    // Test 15: Rilascio delle allocazioni mmap consecutive
    printf("Test 15: Rilascio delle allocazioni mmap consecutive\n");
    for (int i = 0; i < mmap_count; i++) {
        MyAllocator_free(&allocator, mmap_allocations[i], PAGE_SIZE * (i + 1));
    }
    printf("Deallocazione completata delle allocazioni mmap consecutive\n");

    // Test 16: Allocazione di una dimensione non potenza di 2
    printf("Test 16: Allocazione di una dimensione non potenza di 2\n");
    void* ptr9 = MyAllocator_malloc(&allocator, MIN_BUCKET_SIZE / 3);
    if (ptr9) {
        printf("Allocazione riuscita: %p\n", ptr9);
        print_bitmap(&allocator.buddy_allocator.bitmap, allocator.buddy_allocator.num_levels);
        // Deallocazione immediata
        printf("Deallocazione immediata di ptr9: %p\n", ptr9);
        MyAllocator_free(&allocator, ptr9, MIN_BUCKET_SIZE / 3);
    } else {
        printf("Allocazione fallita\n");
    }

    // Test 17: Allocazione di una grande dimensione (più di una pagina)
    printf("Test 17: Allocazione di una grande dimensione (più di una pagina)\n");
    void* ptr10 = MyAllocator_malloc(&allocator, PAGE_SIZE + 1);
    if (ptr10) {
        printf("Allocazione riuscita: %p\n", ptr10);
        // Deallocazione immediata
        printf("Deallocazione immediata di ptr10: %p\n", ptr10);
        MyAllocator_free(&allocator, ptr10, PAGE_SIZE + 1);
    } else {
        printf("Allocazione fallita\n");
    }

    // Deallocazione finale per i puntatori rimasti
    if (ptr5) {
        printf("Deallocazione di ptr5: %p\n", ptr5);
        MyAllocator_free(&allocator, ptr5, MIN_BUCKET_SIZE / 4);
    }
    if (ptr8) {
        printf("Deallocazione di ptr8: %p\n", ptr8);
        MyAllocator_free(&allocator, ptr8, (PAGE_SIZE / 4) - 1);
    }

    printf("Bitmap finale:\n");
    print_bitmap(&allocator.buddy_allocator.bitmap, allocator.buddy_allocator.num_levels);

    return 0;
}