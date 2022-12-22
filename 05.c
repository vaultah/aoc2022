#include <stdio.h>
#include <string.h>

#define CHARS_PER_STACK 3
#define MAX_STACKS 15
#define MAX_LINE_SIZE MAX_STACKS * (CHARS_PER_STACK + 1)
#define MAX_STACK_HEIGHT 500


struct stack
{
    size_t count;
    char boxes[MAX_STACK_HEIGHT];
};
typedef struct stack stack;


void print_all_stacks(stack *stacks) {
    size_t max_height = 0;

    for (size_t i = 0; i < MAX_STACKS; i++)
        if (stacks[i].count > max_height)
            max_height = stacks[i].count;

    for (size_t i = max_height - 1; ; i--) {
        for (size_t j = 0; j < MAX_STACKS; j++)
            printf("%c ", stacks[j].boxes[i] ? stacks[j].boxes[i] : ' ');

        printf("\n");
        if (i == 0)
            break;
    }
}

void print_top_boxes(stack *stacks) {
    for (size_t i = 0; i < MAX_STACKS; i++)
        if (stacks[i].boxes[stacks[i].count - 1])
            printf("%c", stacks[i].boxes[stacks[i].count - 1]);

    printf("\n");
}


int main(int argc, char **argv) {
    char line[MAX_LINE_SIZE];
    unsigned count;
    size_t from, to;
    int box_direction;
    stack stacks[MAX_STACKS];

    if (argc != 2) {
        fprintf(
            stderr,
            "%s: expected exactly one argument (crate mover model, cm9000 or cm9001)\n",
            argv[0]
        );
        return 1;
    } else if (strcmp(argv[1], "cm9000") == 0)
        box_direction = -1;
    else if (strcmp(argv[1], "cm9001") == 0)
        box_direction = 1;
    else {
        fprintf(stderr, "%s: expected cm9000 or cm9001\n", argv[0]);
        return 1;
    }

    memset(stacks, 0, sizeof stacks);

    // Read the initial stack configuration
    while (fgets(line, sizeof line, stdin) != NULL) {
        if (strcmp(line, "\n") == 0)
            break;

        size_t string_length = strcspn(line, "\n");

        for (size_t i = 0; i < MAX_STACKS; i++) {
            size_t stack_index = i * (CHARS_PER_STACK + 1);
            if (stack_index > string_length)
                break;

            char stack_symbol = line[stack_index + 1];
            if (stack_symbol < 'A' || stack_symbol > 'Z')
                continue;

            stacks[i].boxes[stacks[i].count] = stack_symbol;
            stacks[i].count++;
        }
    }

    // Reverse boxes in the stack
    for (size_t i = 0; i < MAX_STACKS; i++) {
        for (size_t j = 0; j < stacks[i].count / 2; j++) {
            char temp = stacks[i].boxes[j];
            stacks[i].boxes[j] = stacks[i].boxes[stacks[i].count - j - 1];
            stacks[i].boxes[stacks[i].count - j - 1] = temp;
        }
    }

    printf("Before moving boxes:\n");
    print_all_stacks(&stacks[0]);
    printf("\n");

    // Process crate moving instructions
    while (fgets(line, sizeof line, stdin) != NULL) {
        sscanf(line, "move %u from %zu to %zu", &count, &from, &to);

        // They are 1-based in the input file
        from--;
        to--;

        size_t i, stop;

        if (box_direction < 0) {
            i = stacks[from].count - 1;
            stop = stacks[from].count - count;
        } else {
            i = stacks[from].count - count;
            stop = stacks[from].count;
        }

        while (box_direction < 0 && i >= stop || box_direction > 0 && i < stop) {
            stacks[to].boxes[stacks[to].count] = stacks[from].boxes[i];
            stacks[to].count++;
            stacks[from].boxes[i] = 0;
            stacks[from].count--;

            if (i == stop)
                break;

            i += box_direction;
        }
    }

    printf("After moving boxes:\n");
    print_all_stacks(&stacks[0]);
    printf("\nTop boxes say ");
    print_top_boxes(&stacks[0]);
    return 0;
}
