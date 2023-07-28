#include <stdio.h>
#include <stdlib.h>

#define MAX_OPERATION_ARG_LENGTH 10
#define ITEMS_RESIZE_STEP 5

typedef struct monkey {
    size_t index;

    long unsigned *items;
    size_t item_count;
    size_t inspections_count;

    // + or *
    char operation_op;
    // -1 for 'old', positive number otherwise
    int operation_arg;
    // divisible by this (very suspiciously prime) number?
    unsigned divisible_by;
    // throw to this monkey if true
    size_t if_divisible_index;
    // throw to this monkey if false
    size_t if_not_divisible_index;
} monkey_t;


monkey_t* get_monkey() {
    monkey_t *new = malloc(sizeof(monkey_t));
    new->inspections_count = 0;
    new->items = NULL;
    new->item_count = 0;
    return new;
}


void resize_items(monkey_t* monkey, size_t capacity) {
    monkey->items = realloc(monkey->items, capacity * sizeof(long unsigned));
}


void free_monkey(monkey_t* monkey) {
    free(monkey->items);
    free(monkey);
}


void display(monkey_t* monkey) {
    printf("Monkey %lu: inspected_count=%lu items=[", monkey->index, monkey->inspections_count);

    size_t i = 0;

    for (; i < monkey->item_count; i++) {
        if (i == monkey->item_count - 1)
            printf("%lu", monkey->items[i]);
        else printf("%lu, ", monkey->items[i]);
    }

    printf("]\n");
}


void monkey_turn(
    monkey_t* monkey,
    monkey_t* true_monkey,
    monkey_t* false_monkey,
    long unsigned worry_modulo,
    unsigned relief_factor
) {
    size_t i = 0;
    long unsigned worry_level;

    for (; i < monkey->item_count; i++) {
        worry_level = monkey->operation_arg == -1 ? monkey->items[i] : monkey->operation_arg;

        switch (monkey->operation_op) {
        case '+':
            worry_level += monkey->items[i];
            break;
        case '*':
            worry_level *= monkey->items[i];
            break;
        }

        worry_level /= relief_factor;
        worry_level %= worry_modulo;

        if (worry_level % monkey->divisible_by == 0)
            true_monkey->items[true_monkey->item_count++] = worry_level;
        else false_monkey->items[false_monkey->item_count++] = worry_level;

        monkey->inspections_count++;
    }

    monkey->item_count = 0;
}


int main(int argc, char* argv[]) {
    monkey_t **monkeys = NULL, *monkey = NULL;
    size_t i, index, round, rounds = 20, most_active = 0, runner_up = 0, total_items = 0;
    char operation_arg[MAX_OPERATION_ARG_LENGTH];
    long unsigned common_multiple = 1, item;
    unsigned relief_factor = 3;

    if (argc > 1 && sscanf(argv[1], "%lu", &rounds) < 1) {
        fprintf(
            stderr,
            "%s: first optional argument must be a positive integer (number of rounds)\n",
            argv[0]
        );
        return 1;
    }

    if (argc > 2 && sscanf(argv[2], "%u", &relief_factor) < 1) {
        fprintf(
            stderr,
            "%s: second optional argument must be a positive integer (relief reduction factor)\n",
            argv[0]
        );
        return 1;
    }

    while (scanf(" Monkey %lu:", &index) > 0) {
        monkey = get_monkey();
        monkey->index = index;

        resize_items(monkey, ITEMS_RESIZE_STEP);
        scanf(" Starting items: %lu", &monkey->items[0]);
        monkey->item_count++;

        for (; scanf(", %lu", &item) > 0; total_items++) {
            if (monkey->item_count % ITEMS_RESIZE_STEP == 0)
                resize_items(monkey, monkey->item_count + ITEMS_RESIZE_STEP);
            monkey->items[monkey->item_count++] = item;
        }

        scanf(" Operation: new = old %c %s", &monkey->operation_op, operation_arg);
        if (sscanf(operation_arg, "%d", &monkey->operation_arg) < 1)
            monkey->operation_arg = -1;

        scanf(" Test: divisible by %u", &monkey->divisible_by);
        common_multiple *= monkey->divisible_by;

        scanf(" If true: throw to monkey %lu", &monkey->if_divisible_index);
        scanf(" If false: throw to monkey %lu", &monkey->if_not_divisible_index);

        monkeys = realloc(monkeys, (index + 1) * sizeof(monkey_t));
        monkeys[index] = monkey;
    }

    for (i = 0; i <= index; i++)
        resize_items(monkeys[i], total_items);

    for (round = 0; round < rounds; round++)
        for (i = 0; i <= index; i++)
            monkey_turn(
                monkeys[i],
                monkeys[monkeys[i]->if_divisible_index],
                monkeys[monkeys[i]->if_not_divisible_index],
                common_multiple,
                relief_factor
            );

    for (i = 0; i <= index; i++) {
        monkey = monkeys[i];
        display(monkey);

        if (monkey->inspections_count > most_active) {
            runner_up = most_active;
            most_active = monkey->inspections_count;
        } else if (monkey->inspections_count > runner_up)
            runner_up = monkey->inspections_count;
    }

    printf("Monkey business level reached %lu\n", most_active * runner_up);

    for (i = 0; i <= index; i++)
        free_monkey(monkeys[i]);

    free(monkeys);
    return 0;
}
