#include "module/utils.c"
#include <stdio.h>
#include <stdlib.h>
#define MAX_PROCESS 20
#define MAX_VIRTUAL 2048
#define MAX_PHYSICAL 1024

int process_num, virtual_num, physical_num;
char tlb_policy[10], page_policy[10], frame_policy[10];

int main()
{
    pasrse_config(tlb_policy, page_policy, frame_policy, &process_num, &virtual_num, &physical_num);
}