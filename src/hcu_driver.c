#include "hcu_driver.h"

void init_hcu(hcu_driver_t *hcu_drv_p) 
{
    printf("Initalizing HCU\n");
    hcu_drv_p->rx_queue = init_queue(RX_QUEUE_SIZE);
    hcu_drv_p->tx_queue = init_queue(TX_QUEUE_SIZE);
    hcu_drv_p->hash_engine_status = HASH_FREE;
}

void *rx_thread_sim(void* rx_queue_p)
{
    hcu_queue_t *queue_p = (hcu_queue_t*) rx_queue_p;
    insert_to_hcu_queue("a", 2, queue_p); 
    insert_to_hcu_queue("b", 2, queue_p); 
    insert_to_hcu_queue("c", 2, queue_p); 
}

void main() 
{
    hcu_driver_t hcu_drv;
    init_hcu(&hcu_drv);

    // Create Threads for sending data to RX queue
    pthread_t rx_thread_id;
    pthread_create(&rx_thread_id, NULL, rx_thread_sim, (void*)hcu_drv.rx_queue);
    pthread_join(rx_thread_id, NULL);
    return;
}
