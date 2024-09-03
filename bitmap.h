#pragma once
#include <stdint.h>

typedef struct {
    char* bits;   // array di bit
    int n_bits;   // numero di bit nella bitmap
} Bitmap;

// funzione che calcola e restituisce la dimensione in bytes necessaria per memorizzare i bits dati in input
int Bitmap_dim(int n_bits);

// funzione che inizializza la bitmap con la dimensione specificata
void Bitmap_init(Bitmap* bitmap, char* bits, int n_bits);

// funzione che imposta un bit specifico nella bitmap ad un valore value (0 o 1)
void Bitmap_set(Bitmap* bitmap, int index, int value);

// funzione che restituisce il valore (0 o 1) di un bit specifico della bitmap
int Bitmap_show(Bitmap* bitmap, int index);