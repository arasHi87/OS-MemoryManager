#include "manager/disk.c"
#include "manager/page.c"
#include "manager/physical.c"
#include "manager/tlb.c"
#include "manager/victim.c"
#include "module/utils.c"
#include <stdio.h>
#include <stdlib.h>

int main()
{
    int process_num, virtual_num, physical_num;
    char tlb_policy[10], page_policy[10], frame_policy[10];

    // parse trace
    pasrse_config(tlb_policy, page_policy, frame_policy, &process_num, &virtual_num, &physical_num);

    char proc, p_proc, proc_k;                        // process, prev process, process which kicked vpn belong
    int pfn, vpn, vpn_k;                              // physical frame, virtual page, vpn kicked
    int number_o, number_i;                           // disk block number out/in
    Result result[MAX_PROCESS];                       // use to calculate EAT and page fault rate
    Disk *disk = Dinit();                             // disk use to save page out vpn
    TLBUffer *TLB = TLBInit();                        // TLB
    PTable *PT = PTInit(virtual_num);                 // page table
    PMemory *phys = PMinit(physical_num);             // physical mnemory
    Queue *victim = QInit();                          // victim page queue
    FILE *trace = fopen("trace.txt", "r");            // trace file
    FILE *trace_out = fopen("trace_output.txt", "w"); // trace output file
    FILE *analysis_out = fopen("analysis.txt", "w");  // analysis output file

    memset(result, 0, sizeof(result));
    while (~fscanf(trace, "Reference(%c, %d)\n", &proc, &vpn))
    {
        // flush TLB if process change
        if (proc != p_proc)
            TLB = TLBInit();
        fprintf(trace_out, "Process %c, ", proc);

        // memory manager
        result[proc - 'A'].Page_hit_amount++; // update page hit
        result[proc - 'A'].TLB_hit_amount++;  // update TLB hit amount

        if (~(pfn = TLBHit(TLB, vpn))) // TLB hit
        {
            fprintf(trace_out, "TLB Hit, %d=>%d\n", vpn, pfn);
            result[proc - 'A'].TLB_hit_success++;
        }
        else // TLB miss
        {
            fprintf(trace_out, "TLB Miss, ");
            if (~(pfn = PTHit(PT, proc - 'A', vpn))) // page hit
                fprintf(trace_out, "Page Hit, %d=>%d\n", vpn, pfn);
            else // page fault
            {
                result[proc - 'A'].Page_hit_fault++;
                fprintf(trace_out, "Page Fault, ");
                if (PTIsInDisk(PT, proc - 'A', vpn))
                {
                    // vpn is in disk, need to move it back to physical memory
                    QNode *node = VPReplace(victim, proc - 'A', vpn, page_policy[0] == 'F',
                                            frame_policy[0] == 'G'); // node which need to kick to disk
                    pfn = PTGetPFN(PT, node->proc, node->vpn);
                    number_i = DPageIn(disk, node->proc, node->vpn);
                    number_o = DPageOut(disk, proc - 'A', vpn);
                    vpn_k = node->vpn;
                    proc_k = node->proc + 'A';
                    PTUpdate(PT, node->proc, node->vpn, 0, 0);
                    if (page_policy[0] != 'C')
                        VPInsert(victim, proc - 'A', vpn);
                }
                else
                {
                    if (~(pfn = PMFindFree(phys))) // free space found
                    {
                        vpn_k = -1;
                        proc_k = proc;
                        number_i = -1;
                        number_o = -1;
                        PMInsert(phys, pfn); // insert vpn into physical memory (only need to mark pfn used)
                        VPInsert(victim, proc - 'A', vpn);     // insert vpn into victim queue
                        PTUpdate(PT, proc - 'A', vpn, pfn, 1); // update page table vpn with pfn
                    }
                    else // no free space, try to kick page in victim queue
                    {
                        QNode *node = VPReplace(victim, proc - 'A', vpn, page_policy[0] == 'F',
                                                frame_policy[0] == 'G'); // node which need to kick to disk
                        pfn = PTGetPFN(PT, node->proc, node->vpn);
                        number_i = DPageIn(disk, node->proc, node->vpn);
                        number_o = -1;
                        vpn_k = node->vpn;
                        proc_k = node->proc + 'A';
                        PTUpdate(PT, node->proc, node->vpn, 0, 0);
                        if (page_policy[0] != 'C')
                            VPInsert(victim, proc - 'A', vpn);
                    }
                }
                PTUpdate(PT, proc - 'A', vpn, pfn, 1); // update asked vpn
                TLBClear(TLB, vpn_k, pfn);             // clear kicked node's TLB and PT
                fprintf(trace_out, "%d, Evict %d of Process %c to %d, %d<<%d\n", pfn, vpn_k, proc_k, number_i, vpn,
                        number_o);
            }
            TLBInsert(TLB, vpn, pfn, (tlb_policy[0] == 'L'));
            TLBHit(TLB, vpn);
            result[proc - 'A'].TLB_hit_amount++;
            result[proc - 'A'].TLB_hit_success++;
            fprintf(trace_out, "Process %c, TLB Hit, %d=>%d\n", proc, vpn, pfn);
        }
        QClockUpdate(victim, proc - 'A', vpn);
        p_proc = proc;
    }

    // calculate EAT and page fault rate
    float eat, page_fault_rate;
    for (int i = 0; i < MAX_PROCESS; i++)
    {
        eat = get_eat(result, i);
        page_fault_rate = get_page_fault_rate(result, i);
        if (eat)
        {
            fprintf(analysis_out, "Process %c, Effective Access Time = %.3f\n", 'A' + i, eat);
            fprintf(analysis_out, "Process %c, Page Fault Rate: %.3f\n", 'A' + i, page_fault_rate);
        }
    }
}