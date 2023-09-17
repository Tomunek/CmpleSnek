#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <time.h>
#include <wchar.h>
#include <locale.h>

struct Snek{
//TODO: implement snek as circular buffer
    int len;
};
typedef struct Snek Snek;

struct Field {
    int width;
    int height;
    char **field;
};
typedef struct Field Field;

void fill_field(Field* field, char symbol){
    for(int i = 0; i < field->height; i++){
        for(int j = 0; j < field->width; j++){
            field->field[i][j] = symbol;
        }
    }
}

int alloc_field(Field* field){
    field->field = calloc(field->height, sizeof(char*));
    if(!field->field){
        return 1;
    }
    for(int i = 0; i < field->height; i++){
        field->field[i] = calloc(field->width, sizeof(char));
        if(!field->field[i]){
            return 1;
        }
    }
    return 0;
}

void dealloc_field(Field* field){
    for(int i = 0; i < field->height; i++){
        free(field->field[i]);
    }
    free(field->field);
}

void display(Field* field, Snek* snek){
    printf("┏");
    for(int i = 0; i < field->width; i++){
        printf("━");
    }
    printf("┓\n");

    printf("┃");
    printf("SCORE: ");
    printf("%d", snek->len);
    int score_len = 0;
    int s = snek->len;
    while (s>0){
        s/=10;
        score_len++;
    }
    for(int i = 0; i < field->width - 7 - score_len; i++){
        printf(" ");
    }
    printf("┃\n");

    printf("┣");
    for(int i = 0; i < field->width; i++){
        printf("━");
    }
    printf("┫\n");

    for(int i = 0; i < field->height; i++){
        printf("┃");
        for(int j = 0; j < field->width; j++){
            printf("%c", field->field[i][j]);
        }
        printf("┃\n");
    }

    printf("┗");
    for(int i = 0; i < field->width; i++){
        printf("━");
    }
    printf("┛\n");
}


int main(int argc, char **argv) {
    setlocale(LC_CTYPE, "");
    struct timespec ts;
    ts.tv_sec = 0;
    ts.tv_nsec = 1000*1000*500;
    Field field;
    Snek snek;
    // TODO: cmd line args handling
    field.width = 20;
    field.height = 10;
    snek.len = 1;
    
    if(alloc_field(&field)){
        fprintf(stderr, "ERROR: buy more RAM!");
        return 1;
    }

    fill_field(&field, '.');

    bool alive = true;
    while(alive){
        //TODO: handle user input
        //TODO: move snek
        display(&field, &snek);
        nanosleep(&ts, &ts);
    }

    dealloc_field(&field);
    return 0;
}