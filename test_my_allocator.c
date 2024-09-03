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
    printf("----------Test 1: Allocazione valida di una piccola dimensione----------\n");
    void* ptr1 = MyAllocator_malloc(&allocator, MIN_BUCKET_SIZE / 2);
    if (ptr1) {
        printf("Allocazione riuscita: %p\n", ptr1);
        print_bitmap(&allocator.buddy_allocator.bitmap, allocator.buddy_allocator.num_levels);
    } else {
        printf("Allocazione fallita\n");
    }

    // Test 2: Allocazione valida di una grande dimensione
    printf("----------Test 2: Allocazione valida di una grande dimensione----------\n");
    void* ptr2 = MyAllocator_malloc(&allocator, PAGE_SIZE);
    if (ptr2) {
        printf("Allocazione riuscita: %p\n", ptr2);
        printf("Deallocazione immediata di ptr2: %p\n", ptr2);
        MyAllocator_free(&allocator, ptr2);
    } else {
        printf("Allocazione fallita\n");
    }

    // Test 3: Allocazione invalida di una dimensione negativa
    printf("----------Test 3: Allocazione invalida di una dimensione negativa----------\n");
    void* ptr3 = MyAllocator_malloc(&allocator, -100);
    if (ptr3) {
        printf("Allocazione riuscita: %p (non dovrebbe succedere)\n", ptr3);
        printf("Deallocazione immediata di ptr3: %p\n", ptr3);
        MyAllocator_free(&allocator, ptr3);
    } else {
        printf("Allocazione fallita (atteso)\n");
    }

    // Test 4: Rilascio di un'allocazione valida piccola
    printf("----------Test 4: Rilascio di un'allocazione valida piccola----------\n");
    if (ptr1) {
        printf("Deallocazione di ptr1: %p\n", ptr1);
        MyAllocator_free(&allocator, ptr1);
        print_bitmap(&allocator.buddy_allocator.bitmap, allocator.buddy_allocator.num_levels);
    }

    // Test 5: Rilascio invalido di un puntatore nullo
    printf("----------Test 5: Rilascio invalido di un puntatore nullo----------\n");
    MyAllocator_free(&allocator, NULL);
    printf("Rilascio di un puntatore nullo (atteso)\n");

    // Test 6: Rilascio invalido di un puntatore non allocato
    printf("----------Test 6: Rilascio invalido di un puntatore non allocato----------\n");
    int rand;
    MyAllocator_free(&allocator, &rand);
    printf("Rilascio di un puntatore non allocato (atteso)\n");

    // Test 7: Allocazione valida ripetuta per riempire il buddy allocator
    printf("----------Test 7: Allocazione valida ripetuta per riempire il buddy allocator----------\n");
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
    printf("----------Test 8: Allocazione di una piccola dimensione con buddy allocator pieno----------\n");
    void* ptr4 = MyAllocator_malloc(&allocator, MIN_BUCKET_SIZE / 2);
    if (ptr4) {
        printf("Allocazione riuscita: %p\n", ptr4);
        print_bitmap(&allocator.buddy_allocator.bitmap, allocator.buddy_allocator.num_levels);
    } else {
        printf("Allocazione fallita (atteso)\n");
    }

    // Deallocazione di tutte le allocazioni fatte nel buddy allocator
    printf("----------Deallocazione di tutte le allocazioni fatte nel buddy allocator----------\n");
    for (int i = 0; i < success_count; i++) {
        MyAllocator_free(&allocator, allocations[i]);
    }
    printf("Deallocazione completata delle allocazioni nel buddy allocator\n");
    print_bitmap(&allocator.buddy_allocator.bitmap, allocator.buddy_allocator.num_levels);

    // Test 9: Allocazione valida di una grande dimensione (esattamente 1/4 della dimensione della pagina - 4 byte)
    printf("----------Test 9: Allocazione valida di una grande dimensione (esattamente 1/4 della dimensione della pagina - 4 byte quindi usa il buddy)----------\n");
    void* ptr5 = MyAllocator_malloc(&allocator, (PAGE_SIZE / 4)-4);
    if (ptr5) {
        printf("Allocazione riuscita: %p\n", ptr1);
        print_bitmap(&allocator.buddy_allocator.bitmap, allocator.buddy_allocator.num_levels);
        printf("Deallocazione di ptr5: %p\n", ptr5);
        MyAllocator_free(&allocator, ptr5);
    } else {
        printf("Allocazione fallita\n");
    }

    // Test 10: Allocazione valida di una grande dimensione (esattamente 1/4 della dimensione della pagina)
    printf("----------Test 10: Allocazione valida di una grande dimensione (esattamente 1/4 della dimensione della pagina quindi usa mmap)----------\n");
    void* ptr6 = MyAllocator_malloc(&allocator, PAGE_SIZE / 4);
    if (ptr6) {
        printf("Allocazione riuscita: %p\n", ptr6);
        printf("Deallocazione immediata di ptr6: %p\n", ptr6);
        MyAllocator_free(&allocator, ptr6);
    } else {
        printf("Allocazione fallita\n");
    }

    // Test 11: Allocazione esatta di una dimensione uguale alla dimensione di una pagina
    printf("----------Test 11: Allocazione esatta di una dimensione uguale alla dimensione di una pagina (usa mmap)----------\n");
    void* ptr7 = MyAllocator_malloc(&allocator, PAGE_SIZE);
    if (ptr7) {
        printf("Allocazione riuscita: %p\n", ptr7);
        printf("Deallocazione immediata di ptr7: %p\n", ptr7);
        MyAllocator_free(&allocator, ptr7);
    } else {
        printf("Allocazione fallita\n");
    }

    // Test 12: Allocazione grande ripetuta (diverse mmap consecutive)
    printf("----------Test 12: Allocazione grande ripetuta (diverse mmap consecutive per raggiungere il limite)----------\n");
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

    // Test 13: Allocazione grande oltre il numero massimo
    printf("----------Test 13: Allocazione grande oltre il numero massimo----------\n");
    void* ptr11 = MyAllocator_malloc(&allocator, PAGE_SIZE);
    if (ptr11) {
        printf("Allocazione riuscita: %p\n (inatteso)", ptr11);
    } else {
        printf("Allocazione fallita (atteso)\n");
    }

    // Test 14: Rilascio delle allocazioni mmap consecutive
    printf("----------Test 14: Rilascio delle allocazioni mmap consecutive----------\n");
    for (int i = 0; i < mmap_count; i++) {
        MyAllocator_free(&allocator, mmap_allocations[i]);
    }
    printf("Deallocazione completata delle allocazioni mmap consecutive\n");

    // Test 15: Allocazione di una dimensione non potenza di 2
    printf("----------Test 15: Allocazione di una dimensione non potenza di 2----------\n");
    void* ptr9 = MyAllocator_malloc(&allocator, MIN_BUCKET_SIZE / 3);
    if (ptr9) {
        printf("Allocazione riuscita: %p\n", ptr9);
        print_bitmap(&allocator.buddy_allocator.bitmap, allocator.buddy_allocator.num_levels);
        printf("Deallocazione immediata di ptr9: %p\n", ptr9);
        MyAllocator_free(&allocator, ptr9);
    } else {
        printf("Allocazione fallita\n");
    }

    printf("------------------------------------\n");

    printf("Bitmap finale:\n");
    print_bitmap(&allocator.buddy_allocator.bitmap, allocator.buddy_allocator.num_levels);

    printf("Numero di allocazioni con mmap attive: %d\n", allocator.mmap_num);

    return 0;
}