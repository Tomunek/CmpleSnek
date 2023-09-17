#include <locale.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <wchar.h>

struct Snek {
    // TODO: implement snek as circular buffer
    int **body;
    int len;
    int x, y;
};
typedef struct Snek Snek;

struct Fruts {
    int **pos;
    int count;
};
typedef struct Fruts Fruts;

struct Field {
    int width;
    int height;
};
typedef struct Field Field;

int alloc_snek(Field *field, Snek *snek) {
    snek->body = calloc(field->height * field->width, sizeof(int *));
    if (!snek->body) {
        return 1;
    }
    for (int i = 0; i < field->height * field->width; i++) {
        snek->body[i] = calloc(2, sizeof(int));
        if (!snek->body[i]) {
            return 1;
        }
    }
    return 0;
}

int alloc_frut(Fruts *fruts) {
    fruts->pos = calloc(fruts->count, sizeof(int *));
    if (!fruts->pos) {
        return 1;
    }
    for (int i = 0; i < fruts->count; i++) {
        fruts->pos[i] = calloc(2, sizeof(int));
        if (!fruts->pos[i]) {
            return 1;
        }
    }
    return 0;
}

void dealloc_snek(Field *field, Snek *snek) {
    for (int i = 0; i < field->height * field->width; i++) {
        free(snek->body[i]);
    }
    free(snek->body);
}

void dealloc_frut(Fruts *fruts) {
    for (int i = 0; i < fruts->count; i++) {
        free(fruts->pos[i]);
    }
    free(fruts->pos);
}

void prepare_snek(Field *field, Snek *snek) {
    snek->body[0][0] = field->width / 2 - 1;
    snek->body[0][1] = field->height / 2;

    snek->body[1][0] = field->width / 2;
    snek->body[1][1] = field->height / 2;

    snek->body[2][0] = field->width / 2 + 1;
    snek->body[2][1] = field->height / 2;
}

void display(Field *field, Snek *snek, Fruts *fruts) {
    printf("┏");
    for (int i = 0; i < field->width; i++) {
        printf("━");
    }
    printf("┓\n");

    printf("┃");
    printf("SCORE: ");
    printf("%d", snek->len);
    int score_len = 0;
    int s = snek->len;
    while (s > 0) {
        s /= 10;
        score_len++;
    }
    for (int i = 0; i < field->width - 7 - score_len; i++) {
        printf(" ");
    }
    printf("┃\n");

    printf("┣");
    for (int i = 0; i < field->width; i++) {
        printf("━");
    }
    printf("┫\n");

    for (int y = 0; y < field->height; y++) {
        printf("┃");
        for (int x = 0; x < field->width; x++) {
            char sym = '.';
            for (int i = 0; i < snek->len; i++) {
                if (snek->body[i][0] == x && snek->body[i][1] == y) {
                    sym = '#';
                }
            }
            for (int i = 0; i < fruts->count; i++) {
                if (fruts->pos[i][0] == x && fruts->pos[i][1] == y) {
                    sym = 'o';
                }
            }
            printf("%c", sym);
        }
        printf("┃\n");
    }

    printf("┗");
    for (int i = 0; i < field->width; i++) {
        printf("━");
    }
    printf("┛\n");
}

int main(int argc, char **argv) {
    setlocale(LC_CTYPE, "");
    struct timespec ts;
    ts.tv_sec = 0;
    ts.tv_nsec = 1000 * 1000 * 500;

    Field field;
    Snek snek;
    Fruts fruts;
    // TODO: cmd line args handling
    field.width = 20;
    field.height = 10;
    fruts.count = 1;
    snek.len = 3;

    if (alloc_snek(&field, &snek) & alloc_frut(&fruts)) {
        fprintf(stderr, "ERROR: buy more RAM!");
        return 1;
    }

    if (alloc_snek(&field, &snek)) {
        fprintf(stderr, "ERROR: buy more RAM!");
        return 1;
    }

    prepare_snek(&field, &snek);
    fruts.pos[0][0] = 3;
    fruts.pos[0][1] = 7;

    bool alive = true;
    while (alive) {
        // TODO: handle user input
        // TODO: move snek
        display(&field, &snek, &fruts);
        nanosleep(&ts, &ts);
    }

    dealloc_snek(&field, &snek);
    dealloc_frut(&fruts);
    return 0;
}