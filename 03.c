#include <stdio.h>
#include <string.h>
#include <stdbool.h>


unsigned priority(char value) {
    // Zero-based (0 - 51)
    return value >= 'a' ? value - 'a' : value - 'A' + 26;
}


int main() {
    const unsigned MAX_LINE_SIZE = 100, GROUP_SIZE = 3, PRIORITIES_RANGE = 52;
    char line[MAX_LINE_SIZE];
    size_t string_length, elfs, i, j;
    bool found;
    unsigned priorities_half[PRIORITIES_RANGE],
             priorities_group[GROUP_SIZE * PRIORITIES_RANGE],
             current_priority,
             total_priorities = 0,
             total_badges_priorities = 0;

    for (;;) {
        memset(priorities_group, 0, sizeof priorities_group);

        for (elfs = 0; elfs < GROUP_SIZE && fgets(line, sizeof line, stdin) != NULL; elfs++) {
            memset(priorities_half, 0, sizeof priorities_half);
            string_length = strcspn(line, "\n");

            for (i = 0, found = false; i < string_length; i++) {
                current_priority = priority(line[i]);
                priorities_group[elfs * PRIORITIES_RANGE + current_priority]++;

                if (i < string_length / 2)
                    priorities_half[current_priority]++;
                else if (!found && priorities_half[current_priority]) {
                    total_priorities += current_priority + 1;
                    found = true;
                }
            }
        }

        for (i = 0; i < PRIORITIES_RANGE; i++) {
            for (j = 0; j < GROUP_SIZE; j++)
                if (!priorities_group[j * PRIORITIES_RANGE + i])
                    break;

            if (j == GROUP_SIZE) {
                total_badges_priorities += i + 1;
                break;
            }
        }

        if (feof(stdin))
            break;
    }

    printf("%u\n", total_priorities);
    printf("%u\n", total_badges_priorities);
    return 0;
}
