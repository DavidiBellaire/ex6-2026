#include <stdio.h>
#include <stdlib.h>
#include "utils.h"
#define INITIAL_CAPACITY 16 //for the getString func

//same func from ex5 :)
char* getString(const char* prompt) {
    printf("%s", prompt);

    int cap = INITIAL_CAPACITY;
    int len = 0;
    char* s = malloc(cap);
    int c;

    if (!s) exit(1);

    while ((c = getchar()) != '\n' && c != EOF) {
        if (len + 1 >= cap) {
            cap *= 2;
            char* tmp = realloc(s, cap);
            if (!tmp) {
                free(s);
                exit(1);
            }
            s = tmp;
        }
        s[len++] = (char)c;
    }

    s[len] = '\0';
    return s;
}

int getInt(const char* prompt) {
    int x;
    printf("%s", prompt);
    while (scanf("%d", &x) != 1) {
        while (getchar() != '\n');   // clean the line
        printf("Invalid input. Try again: ");
    }
    while (getchar() != '\n');       // clean the buffer
    return x;
}