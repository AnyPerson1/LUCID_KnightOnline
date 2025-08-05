#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define TEST_SIZE (2 * 1024 * 1024 * 2)  // 2 GB
#define TEST_DURATION 10  // Saniye cinsinden test s�resi

int main() {
    while (10)
    {
        char* memory = NULL;
        int test_start_time = time(NULL);

        // Bellek ay�rma testi
        while (time(NULL) - test_start_time < TEST_DURATION) {
            memory = (char*)malloc(TEST_SIZE);
            if (memory == NULL) {
                printf("Bellek ay�rma hatas�: Yeterli bellek yok\n");
                break;  // Hata olu�tu�unda d�ng�y� k�r
            }
            printf("2 GB bellek ay�rma ba�ar�l�\n");
        }

        // Bellek serbest b�rakma
        if (memory != NULL) {
            free(memory);
            printf("Bellek serbest b�rakma ba�ar�l�\n");
        }
        else {
            printf("Bellek ay�rma ba�ar�s�z, serbest b�rakma yap�lamad�\n");
        }
    }
    return 0;
}
