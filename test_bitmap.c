#include <assert.h>
#include "bitmap.h"

int main() {
    // test di impostazione e visualizzazione dei bit
    
    char bits[1] = {0}; // bitmap di 8 bit
    Bitmap bitmap;
    Bitmap_init(&bitmap, bits, 8);

    Bitmap_set(&bitmap, 1, 1);
    Bitmap_set(&bitmap, 3, 1);
    Bitmap_set(&bitmap, 5, 1);
    Bitmap_set(&bitmap, 7, 1);
    Bitmap_set(&bitmap, 0, 0);
    Bitmap_set(&bitmap, 2, 0);
    Bitmap_set(&bitmap, 4, 0);
    Bitmap_set(&bitmap, 6, 0);

    assert(Bitmap_show(&bitmap, 1) == 1);
    assert(Bitmap_show(&bitmap, 3) == 1);
    assert(Bitmap_show(&bitmap, 5) == 1);
    assert(Bitmap_show(&bitmap, 7) == 1);
    assert(Bitmap_show(&bitmap, 0) == 0);
    assert(Bitmap_show(&bitmap, 2) == 0);
    assert(Bitmap_show(&bitmap, 4) == 0);
    assert(Bitmap_show(&bitmap, 6) == 0);
    

    printf("Test di impostazione e visualizzazione dei bit superato.\n");
    

    // test di limiti della bitmap
    int index = 9;  // indice fuori dai limiti
    //Bitmap_set(&bitmap, index, 1);  // Questo dovrebbe causare un assert se decommentato
    //int value = Bitmap_show(&bitmap, index);  // Questo dovrebbe causare un assert se decommentato
    
    return 0;
}