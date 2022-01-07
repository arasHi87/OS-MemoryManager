#include "queue.c"
#include <stdlib.h>
#include <string.h>

void VPInsert(Queue *q, int proc, int vpn)
{
    QInq(q, proc, vpn);
}

QNode *VPReplace(Queue *q, int proc, int vpn, int p_policy, int f_policy)
{
    /*
    q:
        Victim page queue
    proc:
        Process want to insert
    vpn:
        Virtual page number want to insert
    p_policy (Page Replacement Policy):
        0 -> CLOCK
        1 -> FIFO
    f_policy (Frame Replacement Policy):
        0 -> LOCAL
        1 -> GLOBAL
     */

    QNode *node;  // node which need to pop to disk
    if (f_policy) // GLOBAL
        node = (p_policy ? QDeq(q) : QUpq(q, proc, vpn, f_policy));
    else // LOCAL
        node = (p_policy ? QRmq(q, proc) : QUpq(q, proc, vpn, f_policy));

    return node;
}