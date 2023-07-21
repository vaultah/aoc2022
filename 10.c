#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LENGTH 20


int main(int argc, char* argv[]) {
    char line[MAX_LINE_LENGTH], crt_line[41];
    int instruction_value, register_value = 1, signal_strength = 0;
    size_t i, instruction_cycles, counter = 1, screen_position;

    while (fgets(line, sizeof line, stdin) != NULL) {
        line[strcspn(line, "\n")] = 0;
        
        if (sscanf(line, "addx %d", &instruction_value)) {
            instruction_cycles = 2;
        } else if (strcmp(line, "noop") == 0) {
            instruction_cycles = 1;
            instruction_value = 0;
        } else {
            fprintf(
                stderr,
                "%s: unrecognized instruction %s\n",
                argv[0],
                line
            );
            return 1;
        }

        for (i = 0; i < instruction_cycles; i++) {
            if (counter % 40 == 1)
                strcpy(crt_line, "........................................");
            else if (counter % 40 == 0)
                printf("%s\n", crt_line);

            screen_position = (counter - 1) % 40;
            if (
                screen_position == register_value - 1
                || screen_position == register_value
                || screen_position == register_value + 1
            )
                crt_line[screen_position] = '#';

            if (counter % 40 == 20 && counter / 40 < 6)
                signal_strength += counter * register_value;
                
            counter++;
        }

        register_value += instruction_value;
    }

    printf("Signal strength: %d\n", signal_strength);
    return 0;
}
