#include "hcu_queue.h"

hcu_queue_t* init_queue(int queue_size) 
{
    hcu_queue_t *queue_p = (hcu_queue_t*) calloc(1,sizeof(hcu_queue_t));
    queue_p->queue_size = queue_size;
    queue_p->rem_queue_size = queue_size;
    return queue_p;
}

void insert_to_hcu_queue(char *data, int data_size, hcu_queue_t *queue_p) 
{
    /* This is the hardware FIFO queue, data_size <= rem_queue_size */
    ll_node_t *node_p;
    node_p = (ll_node_t *) calloc(1, (sizeof(ll_node_t) + data_size));

    pthread_mutex_init(&node_p->lock, NULL);

    if(queue_p->head == NULL) {
        queue_p->head = node_p;
        memcpy(node_p->data, data, data_size);
        return;
    }

    int lock_ret = 1;
    do {
        lock_ret = pthread_mutex_trylock(&queue_p->head->lock);
        if(lock_ret) {
            printf("Failed to acquire lock when adding to hcu_queue %d\n", lock_ret);
        } else {
            printf(" Acquired lock to add to hcu_queue\n");
            memcpy(node_p->data, data, data_size);
            node_p->next = queue_p->head;
            queue_p->head = node_p;
        }
    } while(lock_ret);
    lock_ret = pthread_mutex_unlock(&queue_p->head->next->lock);
}
