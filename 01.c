#include <stdio.h>
#include <string.h>
#include <stdlib.h>


int cmp(const void *a, const void *b) {
    return *(const int *)b - *(const int *)a;
}


int main() {
    const unsigned TOP_N = 3;
    const unsigned MAX_LINE_SIZE = 10;
    const unsigned MAX_ELFS = 300;
    char line[MAX_LINE_SIZE];
    unsigned current_max = 0, current_elf_calories = 0, temp, topn_sum = 0;
    size_t elf_count = 0;
    unsigned calories[MAX_ELFS];

    for (;;) {
        char *result = fgets(line, sizeof line, stdin);

        if (result == NULL || strcmp(line, "\n") == 0) {
            if (current_max < current_elf_calories)
                current_max = current_elf_calories;

            calories[elf_count] = current_elf_calories;
            current_elf_calories = 0;
            elf_count++;
        } else if (result != NULL) {
            sscanf(line, "%d", &temp);
            current_elf_calories += temp;
        }

        if (result == NULL)
            break;
    }

    qsort(calories, elf_count, sizeof calories[0], cmp);
    printf("Elf count: %zu\n", elf_count);
    printf("Max calories: %u\n", current_max);

    printf("Top %u calories:\n", TOP_N);
    for (size_t i = 0; i < TOP_N; i++) {
        topn_sum += calories[i];
        printf("  %u\n", calories[i]);
    }
    printf("Sum of top %u calories: %u\n", TOP_N, topn_sum);
    return 0;
}
