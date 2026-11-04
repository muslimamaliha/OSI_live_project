#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_SIZE 8192
#define PORT 5050

static const char *layer_names[7] = {
    "APPLICATION", "PRESENTATION", "SESSION", "TRANSPORT",
    "NETWORK", "DATA_LINK", "PHYSICAL"
};

/* Add a clearly labelled educational header for one simulated OSI layer. */
static int add_header(char *data, size_t capacity, int layer_index) {
    char temp[BUFFER_SIZE];
    int n = snprintf(temp, sizeof(temp), "<L%d:%s>", 7 - layer_index,
                     layer_names[layer_index]);
    if (n < 0 || (size_t)n + strlen(data) + 1 > capacity) return 0;
    strncat(temp, data, sizeof(temp) - strlen(temp) - 1);
    strncpy(data, temp, capacity - 1);
    data[capacity - 1] = '\0';
    return 1;
}

static int remove_header(char *data, int layer_number) {
    char expected[64];
    int idx = 7 - layer_number;
    snprintf(expected, sizeof(expected), "<L%d:%s>", layer_number, layer_names[idx]);
    size_t n = strlen(expected);
    if (strncmp(data, expected, n) != 0) return 0;
    memmove(data, data + n, strlen(data + n) + 1);
    return 1;
}

#endif
