# deadlock-detect

A C project for Computer Systems course for detecting deadlocks.

Given input such as the file test_case.txt:

0 0 1  
2 1 0  
3 8 9  
4 9 8  
5 5 9  
10 10 5  
11 11 5  


where column one denotes the process ID, column 2 denotes the file ID which is currently locked by the process, and column 3 denotes the file ID which is needed. 
E.g. in this input, process 0 has locked file 0, and is requesting access to file 1, and process 2 has locked file 1 and is requesting access to file 0.
These accesses and locks are cyclic and thus a deadlock occurs. The output for running ./detect -f test_case.txt would be:

Processes 7  
Files 7  
Deadlock detected  
Terminate 0 3  

since there are 7 unique processes, 7 unique file IDs, and two deadlocks. The program finds the files that are in deadlocks, and outputs the lowest ID in each cycle which would break the deadlock.
