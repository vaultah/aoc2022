#include <stdio.h>


int main() {
    const unsigned MAX_LINE_SIZE = 50;
    char line[MAX_LINE_SIZE];
    unsigned first_a, first_b, second_a, second_b, contained_count = 0, overlapping_count = 0;

    while (fgets(line, sizeof line, stdin) != NULL) {
        sscanf(line, "%u-%u,%u-%u", &first_a, &first_b, &second_a, &second_b);
        if (first_a >= second_a && first_b <= second_b || second_a >= first_a && second_b <= first_b)
            contained_count++;

        if (first_a <= second_a && first_b >= second_a || second_a <= first_a && second_b >= first_a)
            overlapping_count++;
    }

    printf("%u\n", contained_count);
    printf("%u\n", overlapping_count);
    return 0;
}
