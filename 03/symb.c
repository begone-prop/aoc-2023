#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <err.h>

#define IS_SYMBOL(C) !(((C) >= '0' && (C) <= '9') || (C) == '.')
#define NEIGH_SIZE 8

typedef struct symbol {
    char symb;
    size_t x, y;
    size_t size;
    int neigh[NEIGH_SIZE];
} symbol;

typedef struct arena {
    char *data;
    size_t size;
    size_t width, height;
} arena;

int to_index(int x, int y) {
    if(x == -1 && y == -1) return 0;
    if(x == 0 && y == -1) return 1;
    if(x == 1 && y == -1) return 2;
    if(x == -1 && y == 0) return 3;
    if(x == 1 && y == 0) return 4;
    if(x == -1 && y == 1) return 5;
    if(x == 0 && y == 1) return 6;
    if(x == 1 && y == 1) return 7;

    return -1;
}

void neighbours(size_t x, size_t y, symbol *s, const arena *ar) {
    bool seen[8] = {0};
    size_t idx = 0;

    for(int dy = -1; dy <= 1; dy++) {
        for(int dx = -1; dx <= 1; dx++) {
            if(dy == 0 && dx == 0) continue;

            int ny = (int)y + dy;
            int nx = (int)x + dx;

            if(nx < 0 || (size_t)nx >= ar->width) continue;
            if(ny < 0 || (size_t)ny >= ar->height) continue;

            char c = ar->data[nx + ny * ar->width];

            if(isdigit(c) && !seen[idx]) {
                int i = nx + ny * ar->width;
                int len = 0;

                for(; i >= 0 && isdigit(ar->data[i]); i--, len++) {
                    if(!dy) continue;
                    int k = to_index(dx - len, dy);
                    if(k == -1) continue;
                    seen[k] = true;
                }

                int start = i + 1;

                i = nx + ny * ar->width;
                len = 0;
                for(; i < ar->size && isdigit(ar->data[i]); i++, len++) {
                    if(!dy) continue;
                    int k = to_index(dx + len, dy);
                    if(k == -1) continue;
                    seen[k] = true;
                }

                int number = strtol(&ar->data[start], NULL, 10);
                s->neigh[s->size++] = number;
            }
            seen[idx++] = true;
        }
    }
}

arena file_to_charp(const char *path) {
    int fd = open(path, O_RDONLY);
    if(fd == -1) err(1, "Failed to open %s", path);

    struct stat sbuff;
    fstat(fd, &sbuff);
    char *data = mmap(NULL, sbuff.st_size, PROT_READ, MAP_PRIVATE, fd, 0);

    char *s = data;
    size_t height = 0;
    while(*s) { if(*s++ == '\n') height++; }

    s = data;
    while(*s++ != '\n') ;
    size_t width = s - data;

    return (arena){data, sbuff.st_size, width, height};
}

int main(void) {
    arena ar = file_to_charp("./input");

    long sum1 = 0;
    long sum2 = 0;

    for(size_t y = 0; y < ar.height; y++) {
        for(size_t x = 0; x < ar.width - 1; x++) {
            char c = ar.data[x + y * ar.width];
            if(!IS_SYMBOL(c)) continue;

            symbol s = (symbol){.symb = c, .x = x, .y = y, .size = 0};
            neighbours(x, y, &s, &ar);

            for(size_t idx = 0; idx < s.size; idx++) {
                sum1 += s.neigh[idx];
            }

            if(s.symb == '*' && s.size == 2) {
                long ratio = 1;
                for(size_t idx = 0; idx < s.size; idx++) {
                    ratio *= s.neigh[idx];
                }

                sum2 += ratio;
            }
        }
    }

    printf("Part 1: %zu\n", sum1);
    printf("Part 2: %zu\n", sum2);
    return 0;
}
