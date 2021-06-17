#include "hcu_queue.h"
#include "hcu_hash.h"

#define RX_QUEUE_SIZE 4
#define TX_QUEUE_SIZE 2
#define NUM_HASH_KEYS 2

enum hash_status {
    HASH_BUSY,
    HASH_FREE,
};

typedef struct hcu_driver_ {
    hcu_queue_t *rx_queue;
    hcu_queue_t *tx_queue;
    unsigned char hash_key[NUM_HASH_KEYS][SHA256_DIGEST_LENGTH];
    int num_valid_hash_keys;
    pthread_mutex_t lock;
    enum hash_status hash_engine_status;
} hcu_driver_t;   

ll_node_t *drained_node = NULL;
pthread_mutex_t drained_node_lock = PTHREAD_MUTEX_INITIALIZER;

SHA256_CTX glob_sha_256_ctx;
pthread_mutex_t glob_sha_256_ctx_lock = PTHREAD_MUTEX_INITIALIZER;
