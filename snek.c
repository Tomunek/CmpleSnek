#include <locale.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>

#define KEY_ESC 27
#define KEY_QUIT 'q'
#define KEY_UP 'w'
#define KEY_LEFT 'a'
#define KEY_DOWN 's'
#define KEY_RIGTH 'd'

enum Direction { up,
                 down,
                 left,
                 rigth };
typedef enum Direction Direction;

struct Snake {
    int x, y;
    Direction dir;
    int len;
    int head_index;
    int tail_index;
    int body_capacity;
    int **body;
};
typedef struct Snake Snake;

struct Fruits {
    int **pos;
    int capacity;
    int count;
};
typedef struct Fruits Fruits;

struct Field {
    int width;
    int height;
};
typedef struct Field Field;

bool is_snake(Snake *snake, int x, int y) {
    int current_index = snake->tail_index;
    while (current_index != snake->head_index) {
        if (snake->body[current_index][0] == x && snake->body[current_index][1] == y) {
            return true;
        }
        current_index++;
        if (current_index >= snake->body_capacity) {
            current_index = 0;
        }
    }
    if (snake->body[current_index][0] == x && snake->body[current_index][1] == y) {
        return true;
    }
    return false;
}

bool is_snake_head(Snake *snake, int x, int y) {
    if (snake->x == x && snake->y == y) {
        return true;
    }
    return false;
}

bool is_fruit(Fruits *fruits, int x, int y) {
    for (int i = 0; i < fruits->count; i++) {
        if (fruits->pos[i][0] == x && fruits->pos[i][1] == y) {
            return true;
        }
    }
    return false;
}

int alloc_snake(Field *field, Snake *snake) {
    snake->body = calloc(snake->body_capacity, sizeof(int *));
    if (!snake->body) {
        return 1;
    }
    for (int i = 0; i < snake->body_capacity; i++) {
        snake->body[i] = calloc(2, sizeof(int));
        if (!snake->body[i]) {
            return 1;
        }
    }
    return 0;
}

int alloc_fruits(Fruits *fruits) {
    fruits->pos = calloc(fruits->capacity, sizeof(int *));
    if (!fruits->pos) {
        return 1;
    }
    for (int i = 0; i < fruits->capacity; i++) {
        fruits->pos[i] = calloc(2, sizeof(int));
        if (!fruits->pos[i]) {
            return 1;
        }
    }
    return 0;
}

void dealloc_snake(Field *field, Snake *snake) {
    for (int i = 0; i < snake->body_capacity; i++) {
        free(snake->body[i]);
    }
    free(snake->body);
}

void dealloc_fruits(Fruits *fruits) {
    for (int i = 0; i < fruits->capacity; i++) {
        free(fruits->pos[i]);
    }
    free(fruits->pos);
}

void extend_snake(Snake *snake) {
    snake->head_index++;
    // If end of snake buffer reached, start over
    if (snake->head_index >= snake->body_capacity) {
        snake->head_index = 0;
    }
    snake->body[snake->head_index][0] = snake->x;
    snake->body[snake->head_index][1] = snake->y;
}

void shorten_snake(Snake *snake) {
    snake->tail_index++;
    // If end of snake buffer reached, start over
    if (snake->tail_index >= snake->body_capacity) {
        snake->tail_index = 0;
    }
}

void prepare_snake(Field *field, Snake *snake) {
    snake->body[0][0] = field->width / 2 - 1;
    snake->body[0][1] = field->height / 2;

    snake->body[1][0] = field->width / 2;
    snake->body[1][1] = field->height / 2;

    snake->tail_index = 0;
    snake->head_index = 1;
    snake->len = 3;
    snake->x = field->width / 2 + 1;
    snake->y = field->height / 2;
    snake->dir = rigth;
}

void spawn_fruit(Fruits *fruits, Field *field, Snake *snake) {
    if (fruits->count < fruits->capacity) {
        int x, y;
        do {
            x = rand() % field->width;
            y = rand() % field->height;
        } while (is_fruit(fruits, x, y) || is_snake(snake, x, y) || is_snake_head(snake, x, y));
        fruits->pos[fruits->count][0] = x;
        fruits->pos[fruits->count][1] = y;
        fruits->count++;
    }
}

void remove_fruit(Fruits *fruits, int x, int y) {
    if (fruits->count > 0) {
        bool found = false;
        for (int i = 0; i < fruits->count - 1; i++) {
            if (!found) {
                if (fruits->pos[i][0] == x && fruits->pos[i][1] == y) {
                    found = true;
                    i--;
                }
            } else {
                fruits->pos[i][0] = fruits->pos[i + 1][0];
                fruits->pos[i][1] = fruits->pos[i + 1][1];
            }
        }
        fruits->count--;
    }
}

void prepare_fruits(Fruits *fruits, Field *field, Snake *snake) {
    fruits->count = 0;
    while (fruits->count < fruits->capacity) {
        spawn_fruit(fruits, field, snake);
    }
}

int setup_game(Field *field, Snake *snake, Fruits *fruits) {
    snake->body_capacity = field->width * field->height;

    if (alloc_snake(field, snake)) {
        fprintf(stderr, "ERROR: buy more RAM! (snake can't fit)");
        return 1;
    }

    if (alloc_fruits(fruits)) {
        fprintf(stderr, "ERROR: buy more RAM! (fruits can't fit)");
        return 1;
    }

    prepare_snake(field, snake);
    prepare_fruits(fruits, field, snake);
    return 0;
}

int read_char_if_available(void) {
    int input = 0;
    int bytes_to_read;
    ioctl(STDIN_FILENO, FIONREAD, &bytes_to_read);
    if (bytes_to_read > 0) {
        while (bytes_to_read > 0) {
            input = getchar();
            bytes_to_read--;
        }
    }
    return input;
}

void input_to_snake_dir(Snake *snake, int input) {
    switch (input) {
    case KEY_UP:
        if (snake->dir != down) {
            snake->dir = up;
        }
        break;
    case KEY_LEFT:
        if (snake->dir != rigth) {
            snake->dir = left;
        }
        break;
    case KEY_DOWN:
        if (snake->dir != up) {
            snake->dir = down;
        }
        break;
    case KEY_RIGTH:
        if (snake->dir != left) {
            snake->dir = rigth;
        }
        break;
    default:
        break;
    }
}

bool move_snake(Field *field, Snake *snake, Fruits *fruits) {
    int next_x = snake->x, next_y = snake->y;
    switch (snake->dir) {
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

    if (next_x < 0 || next_x >= field->width || next_y < 0 || next_y >= field->height) {
        // Snake collided with a wall
        // TODO: Handle "wall-less" field (tp to other side on collision)
        return false;
    } else if (is_snake(snake, next_x, next_y)) {
        return false;
    } else if (is_fruit(fruits, next_x, next_y)) {
        extend_snake(snake);
        remove_fruit(fruits, next_x, next_y);
        spawn_fruit(fruits, field, snake);
    } else {
        extend_snake(snake);
        shorten_snake(snake);
    }

    snake->x = next_x;
    snake->y = next_y;
    return true;
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

void display(Field *field, Snake *snake, Fruits *fruits) {
    printf("┏");
    for (int i = 0; i < field->width; i++) {
        printf("━");
    }
    printf("┓\n");

    printf("┃");
    printf("SCORE: ");
    printf("%d", snake->len);
    int score_len = 0;
    int s = snake->len;
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
            if (is_snake(snake, x, y)) {
                sym = '#';
            }
            for (int i = 0; i < fruits->count; i++) {
                if (fruits->pos[i][0] == x && fruits->pos[i][1] == y) {
                    sym = 'o';
                }
            }
            if (is_snake_head(snake, x, y)) {
                switch (snake->dir) {
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
    srand(time(NULL));
    setlocale(LC_CTYPE, "");
    struct timespec ts;
    ts.tv_sec = 0;
    ts.tv_nsec = 1000 * 1000 * 300;

    Field field;
    Snake snake;
    Fruits fruits;
    // TODO: cmd line args handling
    // -w <width>
    // -h <height>
    // -f <fruit count>
    // -n (no wall collision)
    // --help
    field.width = 20;
    field.height = 10;
    fruits.capacity = 2;

    if (setup_game(&field, &snake, &fruits)) {
        return 1;
    }

    setup_console();
    bool alive = true, quit = false;
    while (alive && !quit) {
        int input = read_char_if_available();
        if (input == KEY_ESC || input == KEY_QUIT) {
            quit = true;
        }
        input_to_snake_dir(&snake, input);
        alive = move_snake(&field, &snake, &fruits);
        display(&field, &snake, &fruits);
        nanosleep(&ts, &ts);
    }
    unsetup_console();

    dealloc_snake(&field, &snake);
    dealloc_fruits(&fruits);
    return 0;
}