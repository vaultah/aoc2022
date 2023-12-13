#include <assert.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LENGTH 500
#define ARRAY_GROWTH_STEP 10


enum separator {
   OPENING_BRACKET = -16,
   CLOSING_BRACKET = -8
};


typedef struct node
{
    // separator or integer values
    int value;
    size_t depth;
    struct node *next;
} node_t;


bool is_value(const node_t *node) {
    return node->value >= 0;
}

bool is_list_start(const node_t *node) {
    return node->value == OPENING_BRACKET;
}

bool is_list_end(const node_t *node) {
    return node->value == CLOSING_BRACKET;
}


node_t* new() {
    node_t* node = malloc(sizeof(node_t));
    node->value = INT_MIN;
    node->depth = 0;
    node->next = NULL;
    return node;
}


void destroy(node_t *root) {
    node_t *next;

    for (node_t *node = root; node; ) {
        next = node->next;
        free(node);
        node = next;
    }
}


node_t* parse(const char list[]) {
    node_t *node = NULL, *current = NULL;
    size_t pos = 0, depth = 0;

    for (;;) {
        if (list[pos] == ',') {
            pos++;
        } else if (list[pos] == '\n' || !list[pos]) {
            break;
        }

        if (node == NULL) {
            current = node = new();
        } else {
            current->next = new();
            current = current->next;
        }

        if (list[pos] == '[') {
            current->value = OPENING_BRACKET;
            current->depth = depth;
            depth++;
            pos++;
        } else if (list[pos] == ']') {
            depth--;
            current->depth = depth;
            current->value = CLOSING_BRACKET;
            pos++;
        } else {
            size_t int_length;
            sscanf(&list[pos], "%d%ln", &current->value, &int_length);
            current->depth = depth;
            pos += int_length;
        }
    }
    return node;
}

void print_list(const node_t* start) {
    if (start == NULL) {
        printf("nil\n");
        return;
    }
    if (is_list_start(start))
        printf("[");
    else if (is_list_end(start))
        printf("]");
    else
        printf("%d", start->value);

    printf(" (%lu)", start->depth);

    if (start->next == NULL) {
        printf("\n");
    } else {
        printf(" -> ");
        print_list(start->next);
    }

}


int compare_int_list(const node_t *integer, const node_t *list) {
    assert(is_value(integer));
    assert(is_list_start(list));
    const node_t *listx = list;

    for (; listx; listx = listx->next) {
        if (is_value(listx)) {
            if (listx->value < integer->value)
                return 1;
            else if (listx->value > integer->value)
                return -1;

            listx = listx->next;
            for (;; listx = listx->next) {
                if (is_value(listx) || is_list_start(listx))
                    return -1;
                else if (listx->depth == list->depth)
                    return 0;
            }
        } else if (is_list_end(listx))
            return 1;
    }
    return 0;
}


int base_compare(const node_t *left, const node_t *right, size_t depth) {
    for (;;) {
        for (; left; ) {
            left = left->next;
            if (left->depth == depth + 1 && !is_list_end(left))
                break;
            else if (left->depth == depth && is_list_end(left))
                left = NULL;
        }

        for (; right; ) {
            right = right->next;
            if (right->depth == depth + 1 && !is_list_end(right))
                break;
            else if (right->depth == depth && is_list_end(right))
                right = NULL;
        }

        assert(!left || is_value(left) || is_list_start(left));
        assert(!right || is_value(right) || is_list_start(right));

        if (!left && !right) {
            return 0;
        } else if (!left && right) {
            return -1;
        } else if (left && !right) {
            return 1;
        } else if (is_value(left) && is_value(right)) {
            if (left->value < right->value)
                return -1;
            else if (left->value > right->value)
                return 1;
        } else {
            int result = 0;

            if (is_list_start(left) && is_list_start(right))
                result = base_compare(left, right, depth + 1);
            else if (is_list_start(left))
                result = -compare_int_list(right, left);
            else result = compare_int_list(left, right);

            if (result)
                return result;
        }
    }

    return 0;
}


int compare(const node_t *left, const node_t *right) {
    return base_compare(left, right, 0);
}


int main(int argc, char* argv[]) {
    char line[MAX_LINE_LENGTH];
    size_t ordered_pair_idx_sum = 0, packet_count = 0, divider_idx_product = 1;
    node_t **packets = NULL, *first_divider = parse("[[2]]"), *second_divider = parse("[[6]]");

    for (;;) {
        if (!fgets(line, sizeof line, stdin)) {
            break;
        } else if (strcmp(line, "\n") == 0)
            continue;

        if (packet_count % ARRAY_GROWTH_STEP == 0)
            packets = realloc(packets, (packet_count + ARRAY_GROWTH_STEP) * sizeof(node_t *));

        packet_count++;
        packets[packet_count - 1] = parse(line);

        if (packet_count % 2 != 0)
            continue;

        if (compare(packets[packet_count - 2], packets[packet_count - 1]) == -1)
            ordered_pair_idx_sum += packet_count / 2;
    }

    printf("%lu\n", ordered_pair_idx_sum);

    size_t i, divider_position;
    for (i = 0, divider_position = 0; i < packet_count; i++)
        if (compare(packets[i], first_divider) == -1)
            divider_position++;

    divider_position += 1;
    divider_idx_product *= divider_position;

    for (i = 0, divider_position = 0; i < packet_count; i++)
        if (compare(packets[i], second_divider) == -1)
            divider_position++;

    divider_position += 2;
    divider_idx_product *= divider_position;

    printf("%lu\n", divider_idx_product);

    for (i = 0; i < packet_count; i++)
        destroy(packets[i]);

    free(packets);
    return 0;
}
