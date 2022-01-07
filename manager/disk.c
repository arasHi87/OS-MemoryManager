#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX_DISK 4096

typedef struct
{
    int proc, vpn;
} DPair;

typedef struct
{
    DPair *arr[MAX_DISK];
} Disk;

DPair *DPairMake(int proc, int vpn)
{
    DPair *tmp = (DPair *)malloc(sizeof(DPair));
    tmp->proc = proc;
    tmp->vpn = vpn;
    return tmp;
}

Disk *Dinit()
{
    Disk *tmp = (Disk *)malloc(sizeof(Disk));
    memset(tmp->arr, 0, sizeof(tmp->arr));
    return tmp;
}

int DPageIn(Disk *disk, int proc, int vpn)
{
    for (int i = 0; i < MAX_DISK; i++)
    {
        if (!disk->arr[i])
        {
            disk->arr[i] = DPairMake(proc, vpn);
            return i;
        }
    }
    return -1;
}

int DPageOut(Disk *disk, int proc, int vpn)
{
    for (int i = 0; i < MAX_DISK; i++)
    {
        if (disk->arr[i] && disk->arr[i]->proc == proc && disk->arr[i]->vpn == vpn)
        {
            disk->arr[i] = NULL;
            return i;
        }
    }
    return -1;
}