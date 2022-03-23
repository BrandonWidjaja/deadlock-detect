#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// Definition of a process
typedef struct {
    int pID;
    int fLock;
    int fReq;
    int count;
} process;

/* Structure of a node */
struct node {
    int id, count, end;// Data 
    struct node *next; // Address 
}*head;

/* 
 * Functions to create and display list
 */
struct node* add(int i, int id, struct node *newNode, struct node *temp);
void createList(process* processes, size_t p_len, int currIndex);
void traverseList();
void freeList(struct node* head);
struct node* find(int id);
bool detectCycle();
void findLinks(process* proccesses, process searching, size_t p_len, struct node *temp, struct node *newNode);

void findDistinct(size_t p_len, process* processes); // Print number of distinct processes and files
int computeExecuteTime(size_t p_len, process* proccesses);


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
    process* proccesses = malloc(sizeof(process) * p_size);
    if (proccesses == NULL) {
        fprintf(stderr, "Malloc failure\n");
        exit(EXIT_FAILURE);
    }

    // Input into struct adapted from COMP30023_2022_SM1 C File IO Introduction (v2) University of Melbourne
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
        proccesses[p_len].count = 0;
        proccesses[p_len++].fReq = col3;

    }

    // Print nodes 
    /*
    for (int i = 0; i < p_len; i++) {
        process proccess = proccesses[i];
        printf("%d %d %d\n", proccess.pID, proccess.fLock, proccess.fReq);
    } */

    
    if (fFlag == true){
        findDistinct(p_len, proccesses);
    }
    
    if (eFlag == true){
        printf("Execution time %d\n", computeExecuteTime(p_len, proccesses));
    } else {
        int deadlocked[p_len];
        int numDeadlocks = 0; 
        for (int i = 0; i < p_len; i++){

            bool isDeadlocked = false;
            
              
            int count = 0;

            createList(proccesses, p_len, i);
            
            struct node *temp;
            struct node *tempCompare;

            temp = head;
            
            
            while (temp != NULL){
                tempCompare = head;
                count = 0;
                while (tempCompare != NULL){
                    if (temp->id == tempCompare->id){
                        count++;
                    }
                    tempCompare = tempCompare->next;
                }
                if (count > 1){
                    isDeadlocked = true;
                }
                
                temp = temp->next;          // Move to next node
            }

            if (isDeadlocked == true){
                temp = head;
                int min = temp->id;
                while(temp != NULL){
                    if (temp->id < min){
                        min = temp->id;
                    }
                    temp = temp->next; 
                }
                deadlocked[numDeadlocks] = min;
                numDeadlocks++;
            }
            
            
            //printf("num: %d\n", numDeadlocks);
            freeList(head);
            
        }
        if (numDeadlocks == 0){
            printf("No deadlocks\n");
        } else{
            printf("Deadlock detected\nTerminate ");
        }
        for (int i = 0; i < numDeadlocks; i++){
                printf("%d ", deadlocked[i]);
        }
        printf("\n");
        
    }
    
    free(proccesses);
    fclose(fp);
    
    return 0;
}




// Print number of distinct processes and files
void findDistinct(size_t p_len, process* proccesses){
    int fNum = 0, fCount = 0, fAll[p_len*2];
    
    // Add all files to fAll array
    for (int i = 0; i < p_len; i++){
        process proccess = proccesses[i];
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
int computeExecuteTime(size_t p_len, process* proccesses){
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





/*
 * Create a list of n nodes
 */
void createList(process* proccesses, size_t p_len, int currIndex){
    struct node *newNode = NULL, *temp;
    int i;

    head = (struct node *)malloc(sizeof(struct node));

    // Terminate if memory not allocated
    if(head == NULL)
    {
        printf("Unable to allocate memory.");
        exit(0);
    }

    head->id = proccesses[currIndex].pID; // Link data field with data
    head->count = 0; 
    head->end = 0;
    head->next = NULL; // Link address field to NULL


    // Create n - 1 nodes and add to list
    temp = head;
    process searching = proccesses[currIndex];
    //printf("Start at: %d\n", startIndex);
    for (i = 0; i < p_len; i++){
        //printf("Comparing %d (%d) to %d (%d)\n", searching.pID, searching.fReq, proccesses[i].pID, proccesses[i].fLock);
        if (searching.fReq == proccesses[i].fLock){
            temp = add(i, proccesses[i].pID, newNode, temp);
            for (int j = 0; j < p_len; j++){
                if (proccesses[i].pID == proccesses[j].pID){
                    searching = proccesses[j];
                }
            }
        }
    }
    
    
    traverseList();
    //printf("\n");

}







struct node* add(int i, int id, struct node *newNode, struct node *temp){
    newNode = (struct node *)malloc(sizeof(struct node));

    newNode->id = id; // Link data field of newNode
    newNode->count = 0;
    newNode->next = NULL; // Make sure new node points to NULL 

    temp->next = newNode; // Link previous node with newNode
    
    
    return temp->next;    // Make current node as previous node
  
}





/*
 * Display entire list
 */
void traverseList()
{
    struct node *temp;

    // Return if list is empty 
    if(head == NULL){
        printf("List empty.");
        return;
    }
    
    temp = head;
    while(temp != NULL){
        //printf("%d -> ", temp->id); // Print data of current node
        temp = temp->next;                 // Move to next node
    }
    
}




void freeList(struct node* head){
   struct node* tmp;
   while (head != NULL && head->end != 1){
       tmp = head;
       head = head->next;
       free(tmp);
    }
}




struct node* find(int id) {

   //start from the first link
   struct node* current = head;

   //if list is empty
   if(head == NULL) {
      return NULL;
   }

   //navigate through list
   while(current->id != id) {
	
      //if it is last node
      if(current->next == NULL) {
         return NULL;
      } else {
         //go to next link
         current = current->next;
      }
   }      
	
   //if data found, return the current Link
   return current;
}


