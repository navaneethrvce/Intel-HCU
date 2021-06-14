#include "hcu_queue.h"

#define RX_QUEUE_SIZE 256
#define TX_QUEUE_SIZE 2

enum hash_status {
    HASH_BUSY,
    HASH_FREE,
};

typedef struct hcu_driver_ {
    hcu_queue_t *rx_queue;
    hcu_queue_t *tx_queue;
    enum hash_status hash_engine_status;
} hcu_driver_t;    
