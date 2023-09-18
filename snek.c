#include <locale.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>
#include <wchar.h>

#define ESC 27

enum Direction { up,
                 down,
                 left,
                 rigth };
typedef enum Direction Direction;

struct Snek {
    // TODO: implement snek as circular buffer
    int x, y;
    Direction dir;
    int len;
    int head_index;
    int tail_index;
    int body_capacity;
    int **body;
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
    snek->body = calloc(snek->body_capacity, sizeof(int *));
    if (!snek->body) {
        return 1;
    }
    for (int i = 0; i < snek->body_capacity; i++) {
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
    for (int i = 0; i < snek->body_capacity; i++) {
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

    snek->tail_index = 0;
    snek->head_index = 1;
    snek->len = 3;
    snek->x = field->width / 2 + 1;
    snek->y = field->height / 2;
    snek->dir = rigth;
}

void make_frut(){
    //TODO
}

void prepare_fruts(){
    //TODO
}


int read_char_of_available(void) {
    int input = 0;
    int bytes_to_read;
    ioctl(STDIN_FILENO, FIONREAD, &bytes_to_read);
    if (bytes_to_read > 0) {
        input = getchar();
        bytes_to_read--;
        while (bytes_to_read > 0) {
            getchar();
            bytes_to_read--;
        }
    }
    return input;
}

void input_to_snek_dir(Snek *snek, int input) {
    switch (input) {
    case 'w':
        snek->dir = up;
        break;
    case 'a':
        snek->dir = left;
        break;
    case 's':
        snek->dir = down;
        break;
    case 'd':
        snek->dir = rigth;
        break;
    default:
        break;
    }
}

bool is_snake(Snek *snek, int x, int y) {
    int current_index = snek->tail_index;
    while (current_index != snek->head_index) {
        if (snek->body[current_index][0] == x && snek->body[current_index][1] == y) {
            return true;
        }
        current_index++;
        if (current_index >= snek->body_capacity) {
            current_index = 0;
        }
    }
    if (snek->body[current_index][0] == x && snek->body[current_index][1] == y) {
            return true;
    }
    return false;
}

bool is_snake_head(Snek *snek, int x, int y) {
    if (snek->x == x && snek->y == y) {
        return true;
    }
    return false;
}

bool is_frut(Fruts *fruts, int x, int y) {
    // TODO
    return false;
}

bool move_snek(Field *field, Snek *snek, Fruts *fruts) {
    int next_x = snek->x, next_y = snek->y;
    switch (snek->dir) {
    case up:
        next_y--;
        break;
    case left:
        next_x--;
        break;
    case down:
        next_y++;
        break;
    case rigth:
        next_x++;
        break;
    default:
        break;
    }

    // TODO: check if next space if snek, frut or wall
    // Extend front
    snek->head_index++;
    // If end of snake buffer reached, start over
    if (snek->head_index >= snek->body_capacity) {
        snek->head_index = 0;
    }
    snek->body[snek->head_index][0] = snek->x;
    snek->body[snek->head_index][1] = snek->y;

    // Remove back
    snek->tail_index++;
    // If end of snake buffer reached, start over
    if (snek->tail_index >= snek->body_capacity) {
        snek->tail_index = 0;
    }

    // Move head
    snek->x = next_x;
    snek->y = next_y;
    return 0;
}

void setup_console() {
    static struct termios config;
    tcgetattr(STDIN_FILENO, &config);
    config.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &config);
}

void unsetup_console() {
    static struct termios config;
    tcgetattr(STDIN_FILENO, &config);
    config.c_lflag |= (ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &config);
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
            if (is_snake(snek, x, y)) {
                sym = '#';
            }
            for (int i = 0; i < fruts->count; i++) {
                if (fruts->pos[i][0] == x && fruts->pos[i][1] == y) {
                    sym = 'o';
                }
            }
            if (is_snake_head(snek, x, y)) {
                switch (snek->dir) {
                case up:
                    sym = '^';
                    break;
                case left:
                    sym = '<';
                    break;
                case down:
                    sym = 'v';
                    break;
                case rigth:
                    sym = '>';
                    break;
                default:
                    break;
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
    ts.tv_nsec = 1000 * 1000 * 300;

    Field field;
    Snek snek;
    Fruts fruts;
    // TODO: cmd line args handling
    field.width = 20;
    field.height = 10;
    snek.body_capacity = field.width * field.height;
    fruts.count = 1;

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

    setup_console();
    bool alive = true;
    while (alive) {
        int input = read_char_of_available();
        if (input == ESC) {
            alive = false;
        }
        input_to_snek_dir(&snek, input);
        move_snek(&field, &snek, &fruts);

        // TODO: move snek
        display(&field, &snek, &fruts);
        nanosleep(&ts, &ts);
    }
    unsetup_console();

    dealloc_snek(&field, &snek);
    dealloc_frut(&fruts);
    return 0;
}