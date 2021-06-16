#include "hcu_driver.h"

void init_hcu(hcu_driver_t *hcu_drv_p) 
{
    printf("Initalizing HCU\n");
    hcu_drv_p->rx_queue = init_queue(RX_QUEUE_SIZE);
    hcu_drv_p->tx_queue = init_queue(TX_QUEUE_SIZE);
    hcu_drv_p->hash_engine_status = HASH_FREE;
}

void insert_to_rx_queue(char *data, int data_size, hcu_queue_t *rx_queue_p) 
{   
    int i = 0;
    while(i < data_size) {
        if (rx_queue_p->rem_queue_size == 0) {
            printf ("RX queue full, waiting for drain \n");
            continue;
        }
        if ((data_size - i) < rx_queue_p->rem_queue_size) {
            insert_to_hcu_queue(data + i, data_size-i, rx_queue_p, true);
            i += data_size ;
        } else { // break data into chunks
            pthread_mutex_lock(&rx_queue_p->lock);
            int data_offset = rx_queue_p->rem_queue_size;
            pthread_mutex_unlock(&rx_queue_p->lock);
            insert_to_hcu_queue(data + i, data_offset, rx_queue_p, false); 
            i += data_offset;
        }
    }
}

void *rx_thread_sim(void* rx_queue_p)
{
    hcu_queue_t *queue_p = (hcu_queue_t*) rx_queue_p;
    insert_to_rx_queue("abcdefghijklmnopqrstuvwxyz", 27, queue_p); 
    insert_to_rx_queue("bcdefg", 7, queue_p);   
    insert_to_rx_queue("kruthika", 9, queue_p); 
}

void *rx_queue_drain(void *rx_queue_p)
{
    hcu_queue_t *queue_p = (hcu_queue_t*) rx_queue_p;
    while(1) {
        while(queue_p->head == NULL) {
            //printf("RX queue empty\n");
            continue;
        }
        int lock_ret = 1;
        do {
            pthread_mutex_lock(&queue_p->lock);
            lock_ret = pthread_mutex_trylock(&queue_p->head->lock);
            if(lock_ret) {
                printf("Failed to accquire lock when draining rx queue %d\n", lock_ret);
            } else {
                printf(" Acquired lock to drain rx_queue\n");
                ll_node_t *last_node = queue_p->head;
                ll_node_t *prev_node = NULL;
                while(last_node->next != NULL) {
                    prev_node = last_node;
                    last_node = last_node->next;
                }
                if(prev_node != NULL){
                    prev_node->next = NULL;
                } else {
                    queue_p->head = NULL;
                }
                drained_node = last_node;
            }
            pthread_mutex_unlock(&queue_p->lock);
        } while(lock_ret);
        if(queue_p->head == NULL) {
            pthread_mutex_unlock(&drained_node->lock);
        }else {
            pthread_mutex_unlock(&queue_p->head->lock);
        }
        pthread_mutex_lock(&queue_p->lock);
        queue_p->rem_queue_size += drained_node->data_size;
        pthread_mutex_unlock(&queue_p->lock);
        printf("Drained %s\n",drained_node->data);
    }
}

void main() 
{
    hcu_driver_t hcu_drv;
    init_hcu(&hcu_drv);

    // Create Threads for sending data to RX queue
    pthread_t rx_thread_id;
    pthread_create(&rx_thread_id, NULL, rx_thread_sim, (void*)hcu_drv.rx_queue);
    //pthread_join(rx_thread_id, NULL);

    pthread_t rx_drain_thread;
    pthread_create(&rx_drain_thread, NULL, rx_queue_drain, (void*)hcu_drv.rx_queue);
    //pthread_join(rx_drain_thread, NULL);
    return;
}
