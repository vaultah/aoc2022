#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SLEN(s) (sizeof(s)/sizeof(s[0]))
#define CD_PREFIX "$ cd "
#define CD_PREFIX_LEN SLEN(CD_PREFIX) - 1
#define LS_PREFIX "$ ls"
#define LS_PREFIX_LEN SLEN(LS_PREFIX) - 1
#define LS_DIR_PREFIX "dir "
#define LS_DIR_PREFIX_LEN SLEN(LS_DIR_PREFIX) - 1
#define MAX_LINE_LENGTH 50
#define MAX_DIRNAME_LENGTH 15
#define MAX_FILESYSTEM_CAPACITY 70000000
#define UPDATE_FILE_SIZE 30000000


typedef struct directory
{
    size_t size;
    char name[MAX_DIRNAME_LENGTH];
    struct directory *parent, *child, *sibling;
} directory;


directory* make_directory() {
    return calloc(1, sizeof(directory));
}


directory* add_subdirectory(directory *d) {
    directory *rv = make_directory();
    rv->parent = d;

    if (d->child == NULL)
        d->child = rv;
    else {
        d = d -> child;
        while (d->sibling != NULL)
            d = d->sibling;

        d->sibling = rv;
    }

    return rv;
}


directory* get_subdirectory(directory *d, char *name) {
    d = d->child;

    while (d != NULL) {
        if (strcmp(d->name, name) == 0)
            break;

        d = d->sibling;
    }

    return d;
}


directory* add_file(directory *d, size_t size) {
    while (d != NULL) {
        d->size += size;
        d = d->parent;
    }
}


size_t sum_folders_under(directory *d, size_t maxsize) {
    size_t sum = 0;

    if (d->size <= maxsize)
        sum += d->size;

    if (d->child != NULL)
        sum += sum_folders_under(d->child, maxsize);

    if (d->sibling != NULL)
        sum += sum_folders_under(d->sibling, maxsize);

    return sum;
}


size_t smallest_greater_than(directory *d, size_t minsize) {
    size_t rv = -1, temp;

    if (d->child != NULL)
        if ((temp = smallest_greater_than(d->child, minsize)) < rv)
            rv = temp;

    if (d->sibling != NULL)
        if ((temp = smallest_greater_than(d->sibling, minsize)) < rv)
            rv = temp;

    if (d->size >= minsize && d->size < rv)
        rv = d->size;

    return rv;
}


void destroy(directory *d) {
    if (d->child != NULL)
        destroy(d->child);

    if (d->sibling != NULL)
        destroy(d->sibling);

    free(d);
}


int main() {
    char line[MAX_LINE_LENGTH], dirname[MAX_DIRNAME_LENGTH], *pos;
    directory *current_directory = NULL, *root = NULL, *temp;

    fgets(line, sizeof line, stdin);

    while (!feof(stdin)) {
        // Process cd instructions
        for (;;) {
            line[strcspn(line, "\n")] = 0;

            if ((pos = strstr(line, CD_PREFIX)) == NULL)
                break;

            assert(pos - line == 0);
            // The remainder of the line is the name of the directory
            strcpy(dirname, pos + CD_PREFIX_LEN);

            if (strcmp(dirname, "/") == 0) {
                assert(current_directory == NULL);
                assert(root == NULL);
                root = make_directory();
                strcpy(root->name, dirname);
                current_directory = root;
            } else if (strcmp(dirname, "..") == 0) {
                assert(current_directory->parent != NULL);
                current_directory = current_directory->parent;
            } else {
                assert(current_directory != NULL);
                temp = get_subdirectory(current_directory, dirname);
                assert(temp != NULL);
                current_directory = temp;
            }

            if (fgets(line, sizeof line, stdin) == NULL)
                break;
        }

        // Ensure that what follows is the ls result
        pos = strstr(line, LS_PREFIX);
        assert(pos - line == 0);
        fgets(line, sizeof line, stdin);

        // Process ls result
        for (;;) {
            line[strcspn(line, "\n")] = 0;

            if (line[0] == '$')
                break;

            if ((pos = strstr(line, LS_DIR_PREFIX)) != NULL) {
                assert(pos - line == 0);
                assert(current_directory != NULL);
                temp = add_subdirectory(current_directory);
                // The remainder of the line is the name of the directory
                strcpy(temp->name, pos + LS_DIR_PREFIX_LEN);
            } else {
                size_t file_size;
                // The part before the (first) space is the file size
                line[strcspn(line, " ")] = 0;
                sscanf(line, "%zu", &file_size);
                add_file(current_directory, file_size);
            }

            if (fgets(line, sizeof line, stdin) == NULL)
                break;
        }
    }

    printf("Sum of small folders: %zu\n", sum_folders_under(root, 100000));
    printf(
        "Smallest sufficient folder: %zu\n",
        smallest_greater_than(root, root->size - MAX_FILESYSTEM_CAPACITY + UPDATE_FILE_SIZE)
    );

    destroy(root);
    return 0;
}
