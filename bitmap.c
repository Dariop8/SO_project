#include "bitmap.h"
#include <assert.h>

int Bitmap_dim(int n_bits) {
    // divido per 8 per ottenere i byte necessari, aggiungo 7 per arrotondare per eccesso
    // (1bit + 7)/ 8 = 1 byte, (8bit + 7)/ 8 = 1byte, (9bit + 7)/ 8 = 2byte ...
    return (n_bits + 7) / 8;  
}

void Bitmap_init(Bitmap* bitmap, char* bits, int n_bits) {
    bitmap->bits = bits;
    bitmap->n_bits = n_bits;
    for (int i = 0; i < Bitmap_dim(n_bits); i++) {
        bitmap->bits[i] = 0; // inizializzo tutti i bit a 0
    }
}

// scrittura e lettura dei bit come per arduino

void Bitmap_set(Bitmap* bitmap, int index, int value) {
    int byte_index = index / 8; //indice del byte all'interno di bits
    int bit_index = index % 8; // indice del bit all'interno di byte_index
    assert(byte_index >= 0 && byte_index < Bitmap_dim(bitmap->n_bits)); // indice fuori dai limiti
    if (value) {
        bitmap->bits[byte_index] |= (1 << bit_index); // imposto il bit a 1
    } else {
        bitmap->bits[byte_index] &= ~(1 << bit_index); // imposto il bit a 0
    }
}

int Bitmap_show(Bitmap* bitmap, int index) {
    int byte_index = index / 8;
    int bit_index = index % 8;
    assert(byte_index >= 0 && byte_index < Bitmap_dim(bitmap->n_bits)); // indice fuori dai limiti
    int ris = (bitmap->bits[byte_index] & (1 << bit_index)) != 0; // valore del bit
    return ris; 
}