#include "hcu_queue.h"

hcu_queue_t* init_queue(int queue_size) 
{
    hcu_queue_t *queue_p = (hcu_queue_t*) calloc(1,sizeof(hcu_queue_t));
    queue_p->queue_size = queue_size;
    queue_p->rem_queue_size = queue_size;
    pthread_mutex_init(&queue_p->lock, NULL);
    return queue_p;
}

void insert_to_hcu_queue(char *data, int data_size, hcu_queue_t *queue_p, bool sha_commit, SHA256_CTX *sha256_ctx) 
{
    /* This is the hardware FIFO queue, data_size <= rem_queue_size */
    ll_node_t *node_p;
    node_p = (ll_node_t *) calloc(1, (sizeof(ll_node_t) + data_size));
    pthread_mutex_init(&node_p->lock, NULL);
    pthread_mutex_lock(&node_p->lock);
    node_p->sha_commit =  sha_commit;
    node_p->sha256_ctx = sha256_ctx;
    pthread_mutex_unlock(&node_p->lock);

    pthread_mutex_lock(&queue_p->lock);
    
    if(queue_p->head == NULL) {
        queue_p->head = node_p;
        memcpy(node_p->data, data, data_size);
        node_p->data_size = data_size;
    } else {
        int lock_ret = 1;
        do {
            lock_ret = pthread_mutex_trylock(&queue_p->head->lock);
            if(lock_ret) {
                printf("Failed to acquire lock when adding to hcu_queue %d\n", lock_ret);
            } else {
                printf(" Acquired lock to add to hcu_queue\n");
                memcpy(node_p->data, data, data_size);
                node_p->next = queue_p->head;
                node_p->data_size = data_size;
                queue_p->head = node_p;
            }
        } while(lock_ret);
        lock_ret = pthread_mutex_unlock(&queue_p->head->next->lock);
    }
    queue_p->rem_queue_size -= data_size;
    pthread_mutex_unlock(&queue_p->lock);
}
