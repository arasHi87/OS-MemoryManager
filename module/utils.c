#include <stdio.h>

typedef struct
{
    float TLB_hit_success;
    float TLB_hit_amount;
    float Page_hit_fault;
    float Page_hit_amount;
} Result;

float get_eat(Result *result, int proc)
{

    if (result[proc].TLB_hit_amount)
    {
        float alpha = result[proc].TLB_hit_success / result[proc].TLB_hit_amount;
        return (float)(120 * alpha + 220 * (1 - alpha));
    }
    return 0;
}

float get_page_fault_rate(Result *result, int proc)
{
    if (result[proc].Page_hit_amount)
        return result[proc].Page_hit_fault / result[proc].Page_hit_amount;
    return 0;
}

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