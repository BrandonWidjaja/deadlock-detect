#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct {
    int pID;
    int fLock;
    int fReq;
} node;

void findDistinct(size_t p_len, node* processes);

int main(int argc, char* argv[]) {
    
    int args;
    int filearg=1;

    bool fFlag=false;

    for (args=0; args<argc; args++){
        if (strcmp("-f", argv[args]) == 0){
            fFlag=true;
            //printf("file read\n");
            filearg = ++args;
        }
    }

    // Open file
    FILE* fp = fopen(argv[filearg], "r");
    if (fp == NULL) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    // Read file
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

    // Print nodes 
    /*
    for (int i = 0; i < p_len; i++) {
        node proccess = proccesses[i];
        printf("%d %d %d\n", proccess.pID, proccess.fLock, proccess.fReq);
    } */

    if (fFlag==true){
        findDistinct(p_len, proccesses);
    }

    free(proccesses);
    fclose(fp);

    
    return 0;
}

void findDistinct(size_t p_len, node* proccesses){
    int pNum=0, fNum=0, fCount=0, fAll[p_len*2];

    for (int i = 0; i < p_len; i++){
        node proccess = proccesses[i];
        int j = 0;

        while (j != i && proccesses[j].pID != proccess.pID){
            ++j;
        }
        if (j==i){
            ++pNum;
        }

        fAll[fCount] = proccess.fLock;
        ++fCount;
        fAll[fCount] = proccess.fReq;
        ++fCount;
    }

    for (int i=0; i<fCount; i++){
        int j = 0;
        while (j != i && fAll[j] != fAll[i]){
            ++j;
        }
        if (j==i){
            ++fNum;
        }

        
    }
    printf("Processes %d\nFiles %d\n", pNum, fNum);

   

}

