#include "hcu_driver.h"

void init_hcu(hcu_driver_t *hcu_drv_p) 
{
    printf("Initalizing HCU\n");
    hcu_drv_p->rx_queue = init_queue(RX_QUEUE_SIZE);
    hcu_drv_p->tx_queue = init_queue(TX_QUEUE_SIZE);
    hcu_drv_p->hash_engine_status = HASH_FREE;
    hcu_drv_p->num_valid_hash_keys = 0;
    pthread_mutex_init(&hcu_drv_p->lock, NULL);
}

void insert_to_key_queue(hcu_driver_t *drv_p, char *key) 
{
    pthread_mutex_lock(&drv_p->lock);
    while(drv_p->num_valid_hash_keys > NUM_HASH_KEYS) {
        printf("Looping in drain keys %d \n", drv_p->num_valid_hash_keys);
        continue;
    }
    memcpy(drv_p->hash_key[0], key, SHA256_DIGEST_LENGTH);
    drv_p->num_valid_hash_keys = 1;
    pthread_mutex_unlock(&drv_p->lock);
}

void insert_to_rx_queue(char *data, int data_size, hcu_driver_t *drv_p, SHA256_CTX *sha256_ctx, char *key) 
{   
    int i = 0;
    hcu_queue_t *rx_queue_p = drv_p->rx_queue;

    pthread_mutex_lock(&glob_sha_256_ctx_lock);
    while(i < data_size) {
        if (rx_queue_p->rem_queue_size == 0) {
            printf ("RX queue full, waiting for drain \n");
            continue;
        }
        if ((data_size - i) <= rx_queue_p->rem_queue_size) {
            insert_to_hcu_queue(data + i, data_size-i, rx_queue_p, true, sha256_ctx);
            i += data_size ;
        } else { // break data into chunks
            pthread_mutex_lock(&rx_queue_p->lock);
            int data_offset = rx_queue_p->rem_queue_size;
            pthread_mutex_unlock(&rx_queue_p->lock);
            insert_to_hcu_queue(data + i, data_offset, rx_queue_p, false, sha256_ctx); 
            i += data_offset;
        }
    }
    insert_to_key_queue(drv_p, key);
    pthread_mutex_unlock(&glob_sha_256_ctx_lock);

}

void *rx_thread_sim(void* hcu_driver_p)
{
    hcu_driver_t *drv_p = (hcu_driver_t*) hcu_driver_p;
    hcu_queue_t *queue_p = drv_p->rx_queue;
    //SHA256_CTX sha256_ctx;
    pthread_mutex_lock(&glob_sha_256_ctx_lock);
    SHA256_Init(&glob_sha_256_ctx);
    pthread_mutex_unlock(&glob_sha_256_ctx_lock);
    insert_to_rx_queue("abcdefghijklmnopqrstuvwxyz", 26, drv_p, NULL, "7025ca2706f1339040d8ee922c9671b9d033dd");

    pthread_mutex_lock(&glob_sha_256_ctx_lock);
    SHA256_Init(&glob_sha_256_ctx);
    pthread_mutex_unlock(&glob_sha_256_ctx_lock);
    insert_to_rx_queue("bcdefg", 6, drv_p, NULL, "8452c1f0a16ac40bbb60972fb3e2fde59f0677f0");

    pthread_mutex_lock(&glob_sha_256_ctx_lock);
    SHA256_Init(&glob_sha_256_ctx);
    pthread_mutex_unlock(&glob_sha_256_ctx_lock);
    insert_to_rx_queue("navaneeth", 9, drv_p, NULL, "651725b4da8b88f280777677454b4587a8abc7c30ee41192ae333bcbd7728641"); 
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
        pthread_mutex_lock(&drained_node_lock);
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
        pthread_mutex_unlock(&drained_node_lock);
        while(drained_node != NULL) {
            //printf("Hash Engine busy\n");
        }
    }
}

void *hash_sha_256_thread(void *hcu_drv_p)
{
    while(1) {
        if(drained_node == NULL) {
            continue;
        }

        pthread_mutex_lock(&drained_node_lock);
        printf("Acquired hash engine lock\n");
        hcu_sha_256_hash_gen(drained_node->data, drained_node->data_size, drained_node->sha256_ctx, drained_node->sha_commit);
        ll_node_t *n = drained_node;
        drained_node = NULL;
        pthread_mutex_unlock(&drained_node_lock);
        free(n);
    }
    hcu_driver_t *drv_p = (hcu_driver_t*)hcu_drv_p;
    pthread_mutex_lock(&drv_p->lock);
    printf(" Key is %s", drv_p->hash_key[0]);
    drv_p->num_valid_hash_keys = 0;
    pthread_mutex_unlock(&drv_p->lock);
}

void main() 
{
    hcu_driver_t hcu_drv;
    init_hcu(&hcu_drv);

    pthread_t hash_thread;
    pthread_create(&hash_thread, NULL, hash_sha_256_thread, NULL);
    //pthread_join(hash_thread, NULL);

    // Create Threads for sending data to RX queue
    pthread_t rx_thread_id;
    pthread_create(&rx_thread_id, NULL, rx_thread_sim, (void*)&hcu_drv);
    //pthread_join(rx_thread_id, NULL);

    pthread_t rx_drain_thread;
    pthread_create(&rx_drain_thread, NULL, rx_queue_drain, (void*)hcu_drv.rx_queue);
    pthread_join(rx_drain_thread, NULL);
    
    return;
}
