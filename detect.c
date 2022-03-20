#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// Definition of a process
typedef struct {
    int pID;
    int fLock;
    int fReq;
} node;

void findDistinct(size_t p_len, node* processes); // Print number of distinct processes and files
int computeExecuteTime(size_t p_len, node* proccesses);


int main(int argc, char* argv[]) {
    
    int args, filearg = 1;

    // Flags
    bool fFlag = false, eFlag = false;

    for (args = 0; args < argc; args++){
        if (strcmp("-f", argv[args]) == 0){
            fFlag = true;
            //printf("file read\n");
            filearg = ++args;
        }
        if (strcmp("-e", argv[args]) == 0){
            eFlag = true;
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

    // Input into struct adapted from COMP30023_2022_SM1 C File IO Introduction (v2) University of Melbourne
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

    if (fFlag == true){
        findDistinct(p_len, proccesses);
    }
    if (eFlag == true){
        printf("Execution time %d\n", computeExecuteTime(p_len, proccesses));
    }

    free(proccesses);
    fclose(fp);

    
    return 0;
}

// Print number of distinct processes and files
void findDistinct(size_t p_len, node* proccesses){
    int fNum = 0, fCount = 0, fAll[p_len*2];
    
    // Add all files to fAll array
    for (int i = 0; i < p_len; i++){
        node proccess = proccesses[i];
        fAll[fCount] = proccess.fLock;
        ++fCount;
        fAll[fCount] = proccess.fReq;
        ++fCount;
    }

    // Count number of distinct files in fAll
    for (int i = 0; i < fCount; i++){
        int j = 0;
        while (j != i && fAll[j] != fAll[i]){
            ++j;
        }
        if (j == i){
            ++fNum;
        }
    }

    printf("Processes %zu\nFiles %d\n", p_len, fNum);
}

// Find the minimum execution time
int computeExecuteTime(size_t p_len, node* proccesses){
    int LOCKTIME = 1; // Locking and releasing all files costs 1 time unit
    int maxReqCount = 1;

    if(p_len < 1){
        return 0;
    }

    for (int i = 0; i < p_len; i++){
        int currReqCount = 1;
        
        for (int j = i + 1; j < p_len; j++){
            // Find count of request of highest frequency
            if(proccesses[j].fReq == proccesses[i].fReq){
                ++currReqCount;
                if (currReqCount > maxReqCount){
                  maxReqCount = currReqCount;  
                }
            }
        }
        currReqCount = 0;
    }

    return maxReqCount + LOCKTIME;
}
