#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define TEST_SIZE (2 * 1024 * 1024 * 2)  // 2 GB
#define TEST_DURATION 10  // Saniye cinsinden test süresi

int main() {
    while (10)
    {
        char* memory = NULL;
        int test_start_time = time(NULL);

        // Bellek ayýrma testi
        while (time(NULL) - test_start_time < TEST_DURATION) {
            memory = (char*)malloc(TEST_SIZE);
            if (memory == NULL) {
                printf("Bellek ayýrma hatasý: Yeterli bellek yok\n");
                break;  // Hata oluþtuðunda döngüyü kýr
            }
            printf("2 GB bellek ayýrma baþarýlý\n");
        }

        // Bellek serbest býrakma
        if (memory != NULL) {
            free(memory);
            printf("Bellek serbest býrakma baþarýlý\n");
        }
        else {
            printf("Bellek ayýrma baþarýsýz, serbest býrakma yapýlamadý\n");
        }
    }
    return 0;
}
