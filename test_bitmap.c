#include "bitmap.h"
#include <stdio.h>
#include <assert.h>

void test_Bitmap_dim() {
    assert(Bitmap_dim(0) == 0);
    assert(Bitmap_dim(1) == 1);
    assert(Bitmap_dim(8) == 1);
    assert(Bitmap_dim(9) == 2);
    assert(Bitmap_dim(16) == 2);
    assert(Bitmap_dim(17) == 3);
    printf("test dim OK! \n");
}

void test_Bitmap_init() {
    char bits[2];
    Bitmap bitmap;
    Bitmap_init(&bitmap, bits, 16);
    
    for (int i = 0; i < Bitmap_dim(16); i++) {
        assert(bitmap.bits[i] == 0);
    }
    assert(bitmap.n_bits == 16);
    printf("test init OK! \n");
}

void test_Bitmap_set_show() {
    char bits[2];
    Bitmap bitmap;
    Bitmap_init(&bitmap, bits, 16);

    Bitmap_set(&bitmap, 0, 1);
    assert(Bitmap_show(&bitmap, 0) == 1);
    
    Bitmap_set(&bitmap, 0, 0);
    assert(Bitmap_show(&bitmap, 0) == 0);
    
    Bitmap_set(&bitmap, 8, 1);
    assert(Bitmap_show(&bitmap, 8) == 1);
    
    Bitmap_set(&bitmap, 8, 0);
    assert(Bitmap_show(&bitmap, 8) == 0);
    
    Bitmap_set(&bitmap, 15, 1);
    assert(Bitmap_show(&bitmap, 15) == 1);
    
    Bitmap_set(&bitmap, 15, 0);
    assert(Bitmap_show(&bitmap, 15) == 0);
    
    // Test di indici fuori limite (dovrebbero fallire con assert)
    // Bitmap_set(&bitmap, 16, 1); 
    // Bitmap_show(&bitmap, 16); 

    printf("test set e show OK! \n");
}

int main() {
    test_Bitmap_dim();
    test_Bitmap_init();
    test_Bitmap_set_show();

    printf("tutti i test passati! \n");

    return 0;
}
