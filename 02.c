#include <stdio.h>
#include <assert.h>


/*
Choices are given by
    0: Rock
    1: Paper
    2: Scissors
*/
unsigned score(unsigned int enemy_choice, unsigned int my_choice) {
    assert(enemy_choice < 3 && enemy_choice >= 0);
    assert(my_choice < 3 && my_choice >= 0);
    if (my_choice == enemy_choice) {
        return my_choice + 3 + 1;
    } else if (my_choice - enemy_choice == 1 || my_choice - enemy_choice == -2) {
        return my_choice + 6 + 1;
    } else {
        return my_choice + 0 + 1;
    }
}


int main() {
    const unsigned MAX_LINE_SIZE = 10;
    char line[MAX_LINE_SIZE], raw_enemy_choice, raw_unknown;
    unsigned enemy_choice, total_score_first = 0, total_score_second = 0;

    while (fgets(line, sizeof line, stdin) != NULL) {
        sscanf(line, "%c %c", &raw_enemy_choice, &raw_unknown);
        enemy_choice = raw_enemy_choice - 'A';
        total_score_first += score(enemy_choice, raw_unknown - 'X');

        switch (raw_unknown) {
        case 'X': // Lose
            total_score_second += score(enemy_choice, (enemy_choice + 2) % 3);
            break;
        case 'Y': // Draw
            total_score_second += score(enemy_choice, enemy_choice);
            break;
        case 'Z': // Win
            total_score_second += score(enemy_choice, (enemy_choice + 1) % 3);
            break;

        }
    }
    printf("Total score (first): %d\n", total_score_first);
    printf("Total score (second): %d\n", total_score_second);
    return 0;
}
