#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX_PROCESS 20
#define MAX_VIRTUAL 2048

typedef struct PTable
{
    /*
    Refrenced Bit:
        0 -> the page table entry is not referenced
        1 -> the page table entry is referenced
    Present Bit:
        0 -> the page is on disk
        1 -> the page is in physical memory
     */
    int size;                             // number of virtual page
    int arr[MAX_PROCESS][MAX_VIRTUAL][3]; // 0->ref bit 1->pre bit 2->pfn
} PTable;

PTable *PTInit(int size)
{
    PTable *tmp = (PTable *)malloc(sizeof(PTable));
    tmp->size = size;
    memset(tmp->arr, 0, sizeof(tmp->arr));
    return tmp;
}

int PTHit(PTable *table, int proc, int vpn)
{
    // if vpn is refrenced and is in physical memory
    if (table->arr[proc][vpn][0] && table->arr[proc][vpn][1])
        return table->arr[proc][vpn][2];
    return -1;
}

int PTIsInDisk(PTable *table, int proc, int vpn)
{
    return (table->arr[proc][vpn][0] && !table->arr[proc][vpn][1]);
}

void PTUpdate(PTable *table, int proc, int vpn, int pfn, int present)
{
    table->arr[proc][vpn][0] = 1;
    table->arr[proc][vpn][1] = present;
    table->arr[proc][vpn][2] = pfn;
}

int PTGetPFN(PTable *table, int proc, int vpn)
{
    return table->arr[proc][vpn][2];
}