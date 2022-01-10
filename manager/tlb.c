#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define MAX_TLB 32

struct timespec tms;

typedef struct TPair
{
    int vpn, pfn;
    long time;
} TPair;

typedef struct TLBUffer
{
    TPair *arr[32];
} TLBUffer;

TPair *TPairMake(int vpn, int pfn)
{
    TPair *tmp = (TPair *)malloc(sizeof(TPair));
    clock_gettime(CLOCK_REALTIME, &tms);
    tmp->vpn = vpn;
    tmp->pfn = pfn;
    tmp->time = tms.tv_nsec;
    return tmp;
}

TLBUffer *TLBInit()
{
    TLBUffer *tmp = (TLBUffer *)malloc(sizeof(TLBUffer));
    memset(tmp->arr, 0, sizeof tmp->arr);
    return tmp;
}

int TLBHit(TLBUffer *TLB, int vpn)
{
    // try to find cache in TLB
    for (int i = 0; i < MAX_TLB; i++)
    {
        if (TLB->arr[i] && TLB->arr[i]->vpn == vpn)
        {
            clock_gettime(CLOCK_REALTIME, &tms);
            TLB->arr[i]->time = tms.tv_nsec;
            return TLB->arr[i]->pfn;
        }
    }
    return -1;
}

void TLBInsert(TLBUffer *TLB, int vpn, int pfn, int policy)
{
    // try to find a free TLB sapce
    for (int i = 0; i < MAX_TLB; i++)
    {
        if (!TLB->arr[i])
        {
            TLB->arr[i] = TPairMake(vpn, pfn);
            return;
        }
    }

    // policy 0->RANDOM 1->LRU
    if (policy)
    {
        // LRU need to try to find the minist count, and replace it
        int idx;
        long min = 0x3f3f3f3f3f3f3f3f;
        for (int i = 0; i < MAX_TLB; i++)
            if (TLB->arr[i]->time <= min)
                min = TLB->arr[i]->time, idx = i;
        TLB->arr[idx] = TPairMake(vpn, pfn);
    }
    else // RANDOM just rand and mod 32
        TLB->arr[rand() % 32] = TPairMake(vpn, pfn);
}

void TLBClear(TLBUffer *TLB, int vpn, int pfn)
{
    // Try to find and clear
    for (int i = 0; i < MAX_TLB; i++)
    {
        if (TLB->arr[i] && TLB->arr[i]->vpn == vpn && TLB->arr[i]->pfn == pfn)
        {
            TLB->arr[i] = NULL;
            return;
        }
    }
}