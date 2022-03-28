#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define FILE_LOCKTIME 1 // execution time for locked files is always 1
#define FILES_PER_LINE 2

// Definition of a process
typedef struct {
    int pID;
    int fLock;
    int fReq;
    int count;
    bool searched;
} process;

/* 
 * Forward declarations
 */
void findDistinct(size_t p_len, process* processes); // Print number of distinct processes and files
int computeExecuteTime(size_t p_len, process* proccesses); // Find execution time
void findDeadlocks(size_t p_len, process* proccesses); // Find deadlocks in file
void searchProcess(process* processes, size_t p_len, int currIndex); // Search deadlocks from process
int compare( const void* a, const void* b); // Compare function for QSort


int main(int argc, char* argv[]) {
    
    /* 
     * Input into struct adapted from COMP30023_2022_SM1 C File IO Introduction (v2) 
     * University of Melbourne 
     */
    int args, filearg = 1;

    // Flags
    bool fFlag = false, eFlag = false;

    for (args = 0; args < argc; args++){
        if (strcmp("-f", argv[args]) == 0){
            fFlag = true;
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
    process* proccesses = malloc(sizeof(process) * p_size);
    if (proccesses == NULL) {
        fprintf(stderr, "Malloc failure\n");
        exit(EXIT_FAILURE);
    }

    
    int col1, col2, col3;
    while (fscanf(fp, "%d %d %d", &col1, &col2, &col3) == 3) {
        if (p_len == p_size) {
            p_size *= 2;
            proccesses = realloc(proccesses, sizeof(process) * p_size);
            if (proccesses == NULL) {
                fprintf(stderr, "Realloc failure\n");
                exit(EXIT_FAILURE);
            }
        }   
        proccesses[p_len].pID = col1;
        proccesses[p_len].fLock = col2;
        proccesses[p_len].searched = false;
        proccesses[p_len].count = 0;
        proccesses[p_len++].fReq = col3;

    }

    // functions called based on input flags
    if (fFlag == true){
        findDistinct(p_len, proccesses);
    }

    if (eFlag == true){
        printf("Execution time %d", computeExecuteTime(p_len, proccesses));
    } else { 

        findDeadlocks(p_len, proccesses);
        
    }

    printf("\n");
    free(proccesses);
    fclose(fp);
    
    return 0;
}



/*
 * Print number of distinct processes and files
 */
void findDistinct(size_t p_len, process* proccesses){
    int fNum = 0, fCount = 0; // number of files, count of current file
    int fAll[p_len*FILES_PER_LINE]; // array of unique files
    int i;
    
    // Add all files to fAll array
    for (i = 0; i < p_len; i++){
        process proccess = proccesses[i];
        fAll[fCount] = proccess.fLock;
        ++fCount;
        fAll[fCount] = proccess.fReq;
        ++fCount;
    }

    // Count number of distinct files in fAll
    for (i = 0; i < fCount; i++){
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



/*
 * find the minimum execution time
 */
int computeExecuteTime(size_t p_len, process* proccesses){
    int maxReqCount = 1;

    // 0 execution time if no processes
    if(p_len < 1){
        return 0;
    }

    // find the count of highest frequency requested file 
    for (int i = 0; i < p_len; i++){
        int currReqCount = 1;
        
        for (int j = i + 1; j < p_len; j++){
            
            if(proccesses[j].fReq == proccesses[i].fReq){
                ++currReqCount;
                if (currReqCount > maxReqCount){
                  maxReqCount = currReqCount;  
                }
            }
        }
        currReqCount = 0;
    }

    // execution time is the highest frequency file count + 1
    return maxReqCount + FILE_LOCKTIME;
}



/*
 * finds deadlocks and prints processes that can be terminated to break deadlocks
 */
void findDeadlocks(size_t p_len, process* proccesses){
    int deadLockedMins[p_len]; // smallest process IDs that are part of a cycle
    int deadlockCount = 0; // number of deadlocks
    int i;

    // search for deadlocked cycle from all processes that haven't been searched
    for (i = 0; i < p_len; i++){

        if (proccesses[i].searched == false){

            int deadArray[p_len]; // array of processes in deadlock cycle from current search
            int deadCount = 0; // number of processes in cycle from current search
            int k;

            // search processes connected to process search, and marked deadlocked processes
            searchProcess(proccesses, p_len, i);

            // store processes that are in deadlock cycle
            for (k = 0; k <p_len; k++){
                
                if (proccesses[k].count > 1){
                    
                    deadArray[deadCount] = proccesses[k].pID;
                    proccesses[k].searched = true;
                    deadCount++;
                }
            }
        
            
            // if deadlock detected, store the process with smallest ID in deadLockedMins
            if (deadCount > 1){
                int currMin = deadArray[0];
                bool alreadyRecorded = false;
                for (k = 0; k < deadCount; k++){
                    
                    if (deadArray[k] < currMin){
                        currMin = deadArray[k];
                    }
                }

                // only add process if not already recorded
                for (k = 0; k < deadlockCount; k++){
                    if (currMin == deadLockedMins[k]){
                        alreadyRecorded = true;
                    }
                }
                if (alreadyRecorded == false){
                    deadLockedMins[deadlockCount] = currMin;
                    deadlockCount++;
                }
                
                    
            }

            // reset processes counts (deadlock marks)
            for (k = 0; k <p_len; k++){
                proccesses[k].count = 0;
            }

        }
    }

    // output
    if (deadlockCount == 0){
        printf("No deadlocks");
    } else{
        printf("Deadlock detected\nTerminate");
    }
    // sort output
    qsort(deadLockedMins, deadlockCount, sizeof(int), compare); 

    for (int i = 0; i < deadlockCount; i++){
        printf(" %d", deadLockedMins[i]);
    }
}



/*
 * searches branch from current process, looking for deadlocks and marking processes 
 * that are part of the cycle
 */
void searchProcess(process* proccesses, size_t p_len, int currIndex){
    
    bool found = false; // deadlock found
    int loops = 0; // while loop termination
    int j;
    process searching = proccesses[currIndex]; // repeat search from this process

    proccesses[currIndex].count++; // mark the process as searched
    
    // keep searching until all processes searched, or deadlock found
    while (found == false && loops < p_len + 1){

        for (int i = 0; i < p_len; i++){

            // mark link between processes depending on files
            if (searching.fReq == proccesses[i].fLock){
                // terminate search if deadlock found
                
                proccesses[i].count++; // mark as searched
                
                for (j = 0; j < p_len; j++){
                    if (proccesses[j].count > 2){
                        found = true;
                        break;
                    }
                    // search from process that is linked
                    if (proccesses[i].pID == proccesses[j].pID){
                        searching = proccesses[j];
                    }
                }
                
                
                
            }
        }

        loops++;
    }
}



/*
 * comparison function for quicksort
 */
int compare(const void* a, const void* b){
     int intA = * ( (int*) a );
     int intB = * ( (int*) b );

     if ( intA == intB ){
         return 0;
     }else if( intA < intB ){
         return -1;
     }else return 1;
}