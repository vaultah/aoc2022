#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#define GRID_MAX 200


typedef struct node {
    char elevation;
    size_t index, x, y;
} node_t;


typedef struct queue {
    node_t **queue;
    int left, right, size, capacity;
} queue_t;


void init(queue_t *q, size_t capacity) {
    q->queue = calloc(capacity, sizeof(node_t*));
    q->left = 0;
    q->right = -1;
    q->size = 0;
    q->capacity = capacity;
}


node_t* dequeue(queue_t *q) {
    if (!q->size)
        return NULL;

    node_t *c = q->queue[q->left];
    q->left++;
    q->left %= q->capacity;
    q->size--;
    return c;
}


bool enqueue(queue_t *q, node_t *c) {
    if (q->size == q->capacity)
        return false;

    q->right++;
    q->right %= q->capacity;
    q->queue[q->right] = c;
    q->size++;
    return true;
}


void destroy(queue_t *q) {
    free(q->queue);
}


int main(int argc, char* argv[]) {
    char line[GRID_MAX + 1], grid[GRID_MAX][GRID_MAX];
    size_t rows = 0, cols = 0,
           x, y, start_x, start_y, end_x, end_y,
           shortest_path = -1, path_from_start;
    node_t *node, *next;
    queue_t *queue;
    bool* visited;

    while (fgets(line, sizeof line, stdin) != NULL) {
        cols = strcspn(line, "\n");

        for (x = 0; x < cols; x++) {
            grid[rows][x] = line[x];

            if (grid[rows][x] == 'S') {
                start_x = x;
                start_y = rows;
                grid[rows][x] = 'a';
            }

            if (grid[rows][x] == 'E') {
                end_x = x;
                end_y = rows;
                grid[rows][x] = 'z';
            }
        }

        rows++;
    }

    queue = malloc(sizeof(queue_t));
    init(queue, cols * rows);
    visited = calloc(rows * cols, sizeof(bool));

    for (y = 0; y < rows; y++) {
        for (x = 0; x < cols; x++) {
            if (grid[y][x] != 'a')
                continue;

            node = malloc(sizeof(node_t));
            node->x = x;
            node->y = y;
            node->elevation = grid[y][x];
            node->index = 0;

            enqueue(queue, node);

            while ((node = dequeue(queue)) != NULL) {
                if (node->y == end_y && node->x == end_x) {
                    if (x == start_x && y == start_y)
                        path_from_start = node->index;

                    if (node->index < shortest_path) {
                        shortest_path = node->index;
                    }
                    break;
                }

                visited[node->y * cols + node->x] = true;

                // Left
                if (
                    node->x > 0
                    && grid[node->y][node->x - 1] - grid[node->y][node->x] <= 1
                    && !visited[node->y * cols + node->x - 1]
                ) {
                    next = malloc(sizeof(node_t));
                    next->x = node->x - 1;
                    next->y = node->y;
                    next->elevation = grid[next->y][next->x];
                    next->index = node->index + 1;
                    visited[next->y * cols + next->x] = true;
                    enqueue(queue, next);
                }

                // Up
                if (
                    node->y > 0
                    && grid[node->y - 1][node->x] - grid[node->y][node->x] <= 1
                    && !visited[(node->y - 1) * cols + node->x]
                ) {
                    next = malloc(sizeof(node_t));
                    next->x = node->x;
                    next->y = node->y - 1;
                    next->elevation = grid[next->y][next->x];
                    next->index = node->index + 1;
                    visited[next->y * cols + next->x] = true;
                    enqueue(queue, next);
                }

                // Right
                if (
                    node->x < cols - 1
                    && grid[node->y][node->x + 1] - grid[node->y][node->x] <= 1
                    && !visited[node->y * cols + node->x + 1]
                ) {
                    next = malloc(sizeof(node_t));
                    next->x = node->x + 1;
                    next->y = node->y;
                    next->elevation = grid[next->y][next->x];
                    next->index = node->index + 1;
                    visited[next->y * cols + next->x] = true;
                    enqueue(queue, next);
                }


                // Down
                if (
                    node->y < rows - 1
                    && grid[node->y + 1][node->x] - grid[node->y][node->x] <= 1
                    && !visited[(node->y + 1) * cols + node->x]
                ) {
                    next = malloc(sizeof(node_t));
                    next->x = node->x;
                    next->y = node->y + 1;
                    next->elevation = grid[next->y][next->x];
                    next->index = node->index + 1;
                    visited[next->y * cols + next->x] = true;
                    enqueue(queue, next);
                }

                free(node);
            }

            while ((node = dequeue(queue)) != NULL)
                free(node);

            memset(visited, 0, rows * cols * sizeof(visited[0]));
        }
    }

    printf("Path from S: %lu\n", path_from_start);
    printf("Shortest path from elevation a: %lu\n", shortest_path);

    destroy(queue);
    free(queue);
    return 0;
}
