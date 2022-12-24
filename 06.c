#include <assert.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#define CHARACTER_RANGE 26


typedef struct queue
{
    char frequencies[CHARACTER_RANGE];
    char *queue;
    int left, right, size, capacity;
} queue;


void init(queue *q, size_t capacity) {
    q->queue = calloc(capacity, sizeof(char));
    q->left = 0;
    q->right = -1;
    q->size = 0;
    q->capacity = capacity;
    memset(q->frequencies, 0, sizeof q->frequencies);
}


bool dequeue(queue *q) {
    if (!q->size) {
        fprintf(stderr, "Queue empty\n");
        return false;
    }

    char c = q->queue[q->left];
    q->left++;
    q->left %= q->capacity;
    q->size--;
    q->frequencies[c - 'a']--;
    return true;
}


bool enqueue(queue *q, char c, bool yeet) {
    if (q->size == q->capacity)
        if (yeet) dequeue(q);
        else {
            fprintf(stderr, "Queue full\n");
            return false;
        }

    q->right++;
    q->right %= q->capacity;
    q->queue[q->right] = c;
    q->frequencies[c - 'a']++;
    q->size++;
    return true;
}


bool first_n_distinct(queue *q, size_t n) {
    assert(n <= q->size);
    int i = 0, queue_index;
    char character;

    for (; i < n; i++) {
        queue_index = (q->left + i) % q->capacity;
        character = q->queue[queue_index];
        if (q->frequencies[character - 'a'] != 1)
            break;
    }

    return i == n;
}

void destroy(queue *q) {
    free(q->queue);
}


int main() {
    char c;
    int counter = 0, first_packet = -1, first_message = -1;
    queue packetq, messageq;

    init(&packetq, 4);
    init(&messageq, 14);

    for (; (c = getchar()) != EOF; counter++) {
        enqueue(&packetq, c, true);
        if (first_packet == -1 && packetq.size > 3 && first_n_distinct(&packetq, 4))
            first_packet = counter;

        enqueue(&messageq, c, true);
        if (first_message == -1 && messageq.size > 13 && first_n_distinct(&messageq, 14))
            first_message = counter;
    }

    destroy(&packetq);
    destroy(&messageq);

    printf("%d\n", first_packet + 1);
    printf("%d\n", first_message + 1);
    return 0;
}
