#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_LINE_LENGTH 200


int main() {
    char grid[MAX_LINE_LENGTH][MAX_LINE_LENGTH];
    size_t grid_size, i, j, k;
    bool visible;
    unsigned viewing_distance, visible_count, max_viewing_distance = 0;

    while (fgets(grid[grid_size], sizeof grid[0], stdin) != NULL)
        grid_size++;

    // Start with edge trees
    visible_count = 4 * (grid_size - 1);

    for (i = 1; i < grid_size - 1; i++) {
        for (j = 1; j < grid_size - 1; j++) {
            visible = false;
            viewing_distance = 1;

            // Left
            for (k = 0; grid[i][k] < grid[i][j]; k++) {
                if (k == j - 1) {
                    visible = true;
                    break;
                }
            }

            for (k = j - 1; grid[i][k] < grid[i][j]; k--)
                if (k == 0)
                    break;

            viewing_distance *= j - k;

            // Right
            for (k = grid_size; grid[i][k] < grid[i][j]; k--) {
                if (k == j + 1) {
                    visible = true;
                    break;
                }
            }

            for (k = j + 1; grid[i][k] < grid[i][j]; k++)
                if (k == grid_size - 1)
                    break;

            viewing_distance *= k - j;

            // Up
            for (k = 0; grid[k][j] < grid[i][j]; k++) {
                if (k == i - 1) {
                    visible = true;
                    break;
                }
            }

            for (k = i - 1; grid[k][j] < grid[i][j]; k--)
                if (k == 0)
                    break;

            viewing_distance *= i - k;

            // Down
            for (k = grid_size; grid[k][j] < grid[i][j]; k--) {
                if (k == i + 1) {
                    visible = true;
                    break;
                }
            }

            for (k = i + 1; grid[k][j] < grid[i][j]; k++)
                if (k == grid_size - 1)
                    break;

            viewing_distance *= k - i;

            // Update trackers
            if (visible)
                visible_count++;

            if (viewing_distance > max_viewing_distance)
                max_viewing_distance = viewing_distance;

        }
    }

    printf("%d visible trees\n", visible_count);
    printf("Maximum viewing distance is %d\n", max_viewing_distance);
    return 0;
}
