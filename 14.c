#include <assert.h>
#include <inttypes.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INITIAL_CAPACITY 64
#define GROWTH_FACTOR 2
#define GROWTH_THRESHOLD 0.5
#define MAX_LINE_LENGTH 2000
#define OPENING_X 500
#define OPENING_Y 0


typedef struct {
    bool set;
    uint64_t key;
    int value;
} hashmap_entry;


typedef struct {
    hashmap_entry *entries;
    size_t capacity;
    size_t length;
} hashmap;


typedef struct {
    hashmap *map;
    int min_x, min_y, max_x, max_y;
    bool floorless;
} grid;


int max(int a, int b) {
    return a < b ? b : a;
}


int min(int a, int b) {
    return a > b ? b : a;
}


hashmap* hashmap_init(void) {
    hashmap *map = malloc(sizeof(hashmap));
    map->length = 0;
    map->capacity = INITIAL_CAPACITY;
    map->entries = calloc(map->capacity, sizeof(hashmap_entry));
    return map;
}


void hashmap_destroy(hashmap *map) {
    free(map->entries);
    free(map);
}


uint64_t make_key(int x, int y) {
    return (((uint64_t) x) << 32) | ((uint32_t) y);
}


uint64_t hash_key(uint64_t key) {
    key = (key ^ (key >> 30)) * 0XBF58476D1CE4E5B9ULL;
    key = (key ^ (key >> 27)) * 0X94D049BB133111EBULL;
    key = key ^ (key >> 31);
    return key;
}


hashmap_entry* hashmap_get(hashmap *map, uint64_t key) {
    uint64_t hash = hash_key(key);
    size_t i = hash & (uint64_t)(map->capacity - 1);

    while (map->entries[i].set) {
        if (map->entries[i].key == key) {
            return &map->entries[i];
        }
        i++;
        if (i >= map->capacity)
            i = 0;
    }
    return NULL;
}


uint64_t find_insertion_point(hashmap_entry *entries, size_t capacity, uint64_t key) {
    uint64_t hash = hash_key(key);
    size_t i = hash & (uint64_t)(capacity - 1);

    while (entries[i].set) {
        if (entries[i].key == key) {
            return i;
        }
        i++;
        if (i == capacity)
            i = 0;
    }
    return i;
}


void hashmap_expand(hashmap* map) {
    size_t new_capacity = map->capacity * GROWTH_FACTOR;
    assert(new_capacity > map->capacity);

    hashmap_entry *new_entries = calloc(new_capacity, sizeof(hashmap_entry));
    for (size_t i = 0; i < map->capacity; i++) {
        if (map->entries[i].set) {
            size_t insert_at = find_insertion_point(new_entries, new_capacity, map->entries[i].key);
            new_entries[insert_at].set = true;
            new_entries[insert_at].key = map->entries[i].key;
            new_entries[insert_at].value = map->entries[i].value;
        }
    }

    free(map->entries);
    map->entries = new_entries;
    map->capacity = new_capacity;
}


void hashmap_set(hashmap *map, uint64_t key, int value) {
    if (map->length >=  map->capacity * GROWTH_THRESHOLD)
        hashmap_expand(map);

    size_t insert_at = find_insertion_point(map->entries, map->capacity, key);
    map->entries[insert_at].key = key;
    map->entries[insert_at].value = value;
    map->length += !map->entries[insert_at].set;
    map->entries[insert_at].set = true;
}


bool check_filled(grid *cave, int x, int y) {
    if (!cave->floorless && y == cave->max_y + 2)
        return true;
    hashmap_entry *tile = hashmap_get(cave->map, make_key(x, y));
    return tile && tile->value;
}

int drop_sand(grid *cave) {
    const int start_x = OPENING_X, start_y = OPENING_Y;
    int x = start_x, y = start_y;

    for (;;) {
        if (cave->floorless && y >= cave->max_y)
            return 0;

        if (!check_filled(cave, x, y + 1)) {
            y++;
        } else {
            if (!check_filled(cave, x - 1, y + 1)) {
                x--;
                y++;
            } else {
                if (!check_filled(cave, x + 1, y + 1)) {
                    x++;
                    y++;
                } else {
                    hashmap_set(cave->map, make_key(x, y), 2);
                    return !(!cave->floorless && x == start_x && y == start_y);
                }
            }
        }
    }
}


grid read_cave() {
    grid cave = {
        .map = hashmap_init(),
        .min_x = INT_MAX,
        .min_y = INT_MAX,
        .max_x = INT_MIN,
        .max_y = INT_MIN,
        .floorless = true
    };
    char line[MAX_LINE_LENGTH];
    int prev_x, prev_y, x, y, direction;

    while (fgets(line, sizeof line, stdin)) {
        size_t start = 0;
        int scanned;

        while (sscanf(&line[start], "%d%*[,]%d%*[-> ] %n", &x, &y, &scanned) == 2) {
            cave.min_y = min(cave.min_y, y);
            cave.max_y = max(cave.max_y, y);
            cave.min_x = min(cave.min_x, x);
            cave.max_x = max(cave.max_x, x);

            if (start != 0) {
                assert(prev_x == x || prev_y == y);
                if (prev_x == x) {
                    direction = prev_y < y ? 1 : -1;
                    for (; prev_y != y; prev_y += direction)
                        hashmap_set(cave.map, make_key(prev_x, prev_y), 1);
                } else {
                    direction = prev_x < x ? 1 : -1;
                    for (; prev_x != x; prev_x += direction)
                        hashmap_set(cave.map, make_key(prev_x, prev_y), 1);
                }
            }

            start += scanned;
            prev_x = x;
            prev_y = y;
        }

        hashmap_set(cave.map, make_key(x, y), 1);
        memset(line, 0, sizeof line);
    }

    return cave;
}


void destroy_cave(grid *cave) {
    hashmap_destroy(cave->map);
}


int main(void) {
    grid cave = read_cave();
    size_t drops = 0;

    while (drop_sand(&cave)) drops++;
    printf("Floorless cave: %lu\n", drops);

    cave.floorless = false;

    while (drop_sand(&cave)) drops++;
    printf("Floored cave: %lu\n", drops + 1);

    destroy_cave(&cave);
    return 0;
}