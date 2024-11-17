#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_READINGS 64


typedef struct {
    long x;
    long y;
} sensor;


typedef struct {
    long x;
    long y;
} beacon;


typedef struct {
    sensor *s;
    beacon *b;
    long distance;
} reading;


long distance(long x1, long y1, long x2, long y2) {
    return labs(x1 - x2) + labs(y1 - y2);
}


bool is_covered(reading *sensor_reading, long x, long y) {
    return (
        y <= x - sensor_reading->s->x + sensor_reading->distance + sensor_reading->s->y
        && y >= x - sensor_reading->s->x - sensor_reading->distance + sensor_reading->s->y
        && y <= -x + sensor_reading->s->x + sensor_reading->distance + sensor_reading->s->y
        && y >= -x + sensor_reading->s->x - sensor_reading->distance + sensor_reading->s->y
    );
}


int main(int argc, char *argv[]) {
    long rowy = 2000000;
    long lower_bound = 0, upper_bound = 4000000;

    if (argc > 1 && sscanf(argv[1], "%lu", &rowy) < 1) {
        fprintf(
            stderr,
            "%s: first optional argument must be a positive integer (row number for part 1)\n",
            argv[0]
        );
        return 1;
    }

    if (argc > 2 && sscanf(argv[2], "%lu", &upper_bound) < 1) {
        fprintf(
            stderr,
            "%s: second optional argument must be a positive integer (upper bound for part 2)\n",
            argv[0]
        );
        return 1;
    }

    sensor sensors[MAX_READINGS];
    beacon beacons[MAX_READINGS];
    reading readings[MAX_READINGS];

    size_t count = 0, row_empty_count = 0;
    long minx = LONG_MAX, maxx = LONG_MIN;
    long offsetneg[MAX_READINGS * 2], offsetpos[MAX_READINGS * 2];

    while (
        scanf(
            "Sensor at x=%ld, y=%ld: closest beacon is at x=%ld, y=%ld\n",
            &sensors[count].x,
            &sensors[count].y,
            &beacons[count].x,
            &beacons[count].y
        ) == 4
    ) {
        reading *rdng = &readings[count];
        rdng->s = &sensors[count];
        rdng->b = &beacons[count];
        rdng->distance = distance(rdng->b->x, rdng->b->y, rdng->s->x, rdng->s->y);

        if (sensors[count].x - rdng->distance < minx)
            minx = sensors[count].x - rdng->distance;

        if (sensors[count].x + rdng->distance > maxx)
            maxx = sensors[count].x + rdng->distance;

        // NOTE: expands sensor coverage by 1 point
        offsetneg[count] = rdng->s->x + rdng->distance + rdng->s->y + 1;
        offsetneg[count + 1] = rdng->s->x - rdng->distance + rdng->s->y - 1;
        offsetpos[count] = -rdng->s->x + rdng->distance + rdng->s->y + 1;
        offsetpos[count + 1] = -rdng->s->x - rdng->distance + rdng->s->y - 1;
        count++;
    }

    for (long x = minx; x <= maxx; x++) {
        bool any_covers = false, any_beacon = false;

        for (size_t i = 0; i < count; i++) {
            reading *current_reading = &readings[i];
            if (current_reading->b->y == rowy && current_reading->b->x == x)
                any_beacon = true;

            if (is_covered(current_reading, x, rowy)) {
                any_covers = true;
                break;
            }
        }

        if (any_covers && !any_beacon)
            row_empty_count++;
    }

    printf("%ld\n", row_empty_count);

    for (size_t i = 0; i < count * 2; i++) {
        for (size_t j = 0; j < count * 2; j++) {
            long x = (offsetneg[i] - offsetpos[j]) / 2, y = (offsetneg[i] + offsetpos[j]) / 2;
            if (x < lower_bound || x > upper_bound || y < lower_bound || y > upper_bound)
                continue;

            bool any_covers = false;

            for (size_t i = 0; i < count; i++) {
                if (is_covered(&readings[i], x, y)) {
                    any_covers = true;
                    break;
                }
            }

            if (!any_covers)
                printf("%ld\n", x * 4000000 + y);
        }
    }

    return 0;
}
