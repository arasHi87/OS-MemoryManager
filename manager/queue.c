#include <stdlib.h>
#include <string.h>

typedef struct QNode
{
    int cnt, vpn, proc;
    struct QNode *prev, *next;
} QNode;

typedef struct
{
    /*
    Queue use to maintain FIFO and CLOCK policy
    FIFO:
        GLOBAL:
            just maintain a origin queue
        LOCAL:
            deq until find same process node
    CLOCK
        GLOBAL:
            use clock[0] to be pointer of queue
        LOCAL:
            use whole clock[] to maintain all different process pointer
     */
    QNode *clock[20]; // use to be clock policy index
    QNode *front, *rear;
} Queue;

QNode *QNInit(int proc, int vpn)
{
    QNode *tmp = (QNode *)malloc(sizeof(QNode));
    tmp->vpn = vpn;
    tmp->proc = proc;
    tmp->cnt = 1;
    tmp->next = tmp->prev = NULL;
    return tmp;
}

Queue *QInit()
{
    Queue *tmp = (Queue *)malloc(sizeof(Queue));
    tmp->front = tmp->rear = NULL;
    return tmp;
}

void QInq(Queue *q, int proc, int vpn)
{
    QNode *node = QNInit(proc, vpn);
    if (!q->front)
        q->front = q->rear = node;
    else
    {
        q->rear->next = node;
        node->prev = q->rear;
        q->rear = node;
    }
}

QNode *QDeq(Queue *q)
{
    // Normal FIFO queue pop
    QNode *tmp = NULL;
    if (q->front)
    {
        tmp = q->front;
        q->front = q->front->next;
        if (!q->front)
            q->rear = NULL;
        else
            q->front->prev = NULL;
    }
    return tmp;
}

QNode *QClockFind(Queue *q, int proc, int policy)
{
    // pointer use to move on clock queue
    // policy 1 is GLOBAL or 0 is LOCAL
    QNode *tmp;
    int idx = (policy ? 0 : proc);

    while (1)
    {
        if (!q->clock[idx])
            q->clock[idx] = q->front;
        if (policy || proc == q->clock[idx]->proc) // if is GLOBAL or LOCAL proc equal
        {
            if (!q->clock[idx]->cnt)
            {
                tmp = q->clock[idx];
                q->clock[idx] = q->clock[idx]->next;
                return tmp;
            }
            else
            {
                q->clock[idx]->cnt = 0;
            }
        }
        q->clock[idx] = q->clock[idx]->next;
    }
}

void QClockUpdate(Queue *q, int proc, int vpn)
{
    QNode *node = q->front;
    while (node)
    {
        if (node->proc == proc && node->vpn == vpn)
        {
            node->cnt = 1;
            return;
        }
        node = node->next;
    }
}

QNode *QFIFOFind(Queue *q, int proc)
{
    QNode *tmp = q->front;
    while (tmp)
    {
        if (tmp->proc == proc)
            return tmp;
        tmp = tmp->next;
    }
    return NULL;
}

QNode *QUpq(Queue *q, int proc, int vpn, int policy)
{
    // update node cnt/proc which find
    // and return which page need to move to disk
    QNode *node = QClockFind(q, proc, policy);
    QNode *tmp = QNInit(node->proc, node->vpn);
    node->vpn = vpn;
    node->proc = proc;
    node->cnt = 1;
    return tmp;
}

QNode *QRmq(Queue *q, int proc)
{
    // remove node which found
    // only LOCAL FIFO will use this
    QNode *node = QFIFOFind(q, proc);
    if (node == q->front)
    {
        q->front = q->front->next;
        if (q->front)
            q->front->prev = NULL;
    }
    else if (node == q->rear)
    {
        q->rear = q->rear->prev;
        q->rear->next = NULL;
    }
    else
    {
        node->prev->next = node->next;
        node->next->prev = node->prev;
    }
    return node;
}

void QPrint(Queue *q)
{
    QNode *tmp = q->front;
    while (tmp)
    {
        // printf("%d %d %d %d\n", (tmp->prev) ? tmp->prev->vpn : 0, tmp->vpn, (tmp->next) ? tmp->next->vpn : 0,
        // tmp->cnt),
        //     tmp = tmp->next;
        printf("%d %d %d\n", tmp->proc, tmp->vpn, tmp->cnt);
        tmp = tmp->next;
    }
}