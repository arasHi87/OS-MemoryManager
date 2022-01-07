#include <stdlib.h>
#include <string.h>
#define MAX_PHYSICAL 1024

typedef struct PMemory
{
    int size;
    int arr[MAX_PHYSICAL];
} PMemory;

PMemory *PMinit(int size)
{
    PMemory *tmp = (PMemory *)malloc(sizeof(PMemory));
    tmp->size = size;
    memset(tmp->arr, 0, sizeof(tmp->arr));
    return tmp;
}

int PMFindFree(PMemory *memory)
{
    // return index if find
    for (int i = 0; i < memory->size; i++)
        if (!memory->arr[i])
            return i;
    return -1;
    // return -1 if not found
}

void PMInsert(PMemory *memory, int pfn)
{
    memory->arr[pfn] = 1;
}