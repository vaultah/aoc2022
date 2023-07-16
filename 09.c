#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>


typedef struct {
    int x, y;
} knot;


/*

    ... | sort | uniq | wc -l

    :^)

*/


int main(int argc, char* argv[]) {
    char direction;
    unsigned amount;
    size_t i, j, knot_count;
    knot *knots, *prev, *cur;

    if (argc < 2 || sscanf(argv[1], "%lu", &knot_count) < 1 || knot_count < 1) {
        fprintf(
            stderr,
            "%s: expected exactly one argument (knot count, greater than 0)\n",
            argv[0]
        );
        return 1;
    }

    knots = calloc(knot_count, sizeof(knot));

    while (scanf("%c %u\n", &direction, &amount) == 2) {
        for (i = 0; i < amount; i++) {
            switch (direction) {
            case 'R':
                knots[0].x++;
                break;
            case 'L':
                knots[0].x--;
                break;
            case 'U':
                knots[0].y++;
                break;
            case 'D':
                knots[0].y--;
                break;
            }

            prev = &knots[0];

            for (j = 1; j < knot_count; j++) {
                cur = &knots[j];

                if (cur->x == prev->x || cur->y == prev->y) {
                    cur->x += (prev->x - cur->x) / 2;
                    cur->y += (prev->y - cur->y) / 2;
                } else if ((cur->x - prev->x) / 2 || (cur->y - prev->y) / 2) {
                    cur->x += prev->x > cur->x ? 1 : -1;
                    cur->y += prev->y > cur->y ? 1 : -1;
                }

                prev = cur;
            }

            printf("%d %d\n", prev->y, prev->x);
        }

    }

    free(knots);
    return 0;
}
