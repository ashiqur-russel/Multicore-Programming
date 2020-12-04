# Multicore-Programming
## Task 1 : statistics_norm.c

The program statistic norm does the following calculations: First, the function statistic norm matrix() searches for each column of the matrix source matrix the maximum value of this column (maxj ). The respective factor is inverted ( 1 ) and stored into the diagonal matrix norm matrix. Afterwards, the maxj
diagonal matrix is multiplied with the source matrix in the function matrix multiplicatoin() to obtain a matrix dest matrix in which the values of the columns are normalized between 0 and 1.
### Task:
Parallelize the calculations int the functions statistic norm matrix() and matrix multiplication() using OpenMP.

## Task 2 : linked_list.c

The program in the file linked list.c contains a sequential implementation of a linked list with integer elements. The data structure of the linked list list t is initialized with the function init list() and the allocated memory is freed with the function destroy list(). Each number may only appear once in the list. The following operations are possible with the list:
• Insert an element into the list with the function list add(). If the element is already present, the element is not inserted. If the element is inserted, the function returns true, otherwise false.
• Remove an element from the list with the function list remove(). If the element is removed, the function returns true, otherwise false.
• Find an element in the list with the function list find(). If an element is in the list, the function returns true, otherwise false.
### Task:
Modify the source code such that a parallel access to the list is possible by the functions list add(), list find() and list remove(). For this, you have to synchronize the access to the list elements. For your implementation use the OpenMP Locks and assign each element of the list a separate lock to enable the parallel access of multiple threads to the list.
