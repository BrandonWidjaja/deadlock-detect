#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct {
    int pID;
    int fLock;
    int fReq;
} node;

int main(int argc, char* argv[]) {
    int args;
    int filearg=1;
    for (args=0; args<argc; args++){
        if (strcmp("-f", argv[args]) == 0){
            printf("file read\n");
            filearg = ++args;
        }
    }
    // Open file
    FILE* fp = fopen(argv[filearg], "r");
    if (fp == NULL) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }
    // Read into memory
    size_t p_len = 0, p_size = 2;
    node* proccesses = malloc(sizeof(node) * p_size);
    if (proccesses == NULL) {
        fprintf(stderr, "Malloc failure\n");
        exit(EXIT_FAILURE);
    }
    int col1, col2, col3;

    while (fscanf(fp, "%d %d %d", &col1, &col2, &col3) == 3) {
        if (p_len == p_size) {
            p_size *= 2;
            proccesses = realloc(proccesses, sizeof(node) * p_size);
            if (proccesses == NULL) {
                fprintf(stderr, "Realloc failure\n");
                exit(EXIT_FAILURE);
            }
        }   
        proccesses[p_len].pID = col1;
        proccesses[p_len].fLock = col2;
        proccesses[p_len++].fReq = col3;
    }
    // Print values
    for (int i = 0; i < p_len; i++) {
        node proccess = proccesses[i];
        printf("%d %d %d\n", proccess.pID, proccess.fLock, proccess.fReq);
    }
    free(proccesses);
    fclose(fp);
    return 0;
}