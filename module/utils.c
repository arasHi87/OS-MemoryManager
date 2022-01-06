#include <stdio.h>

void pasrse_config(char *tlb, char *page, char *frame, int *ps_num, int *vl_num, int *pl_num)
{
    FILE *config = fopen("sys_config.txt", "r");

    // parse system config
    fscanf(config, "TLB Replacement Policy: %s\n", tlb);
    fscanf(config, "Page Replacement Policy: %s\n", page);
    fscanf(config, "Frame Allocation Policy: %s\n", frame);
    fscanf(config, "Number of Processes: %d\n", ps_num);
    fscanf(config, "Number of Virtual Page: %d\n", vl_num);
    fscanf(config, "Number of Physical Frame: %d", pl_num);
}