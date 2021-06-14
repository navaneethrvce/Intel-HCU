#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <pthread.h>
#include <stdio.h>

typedef struct ll_node_ {
    struct ll_node_ *next;
    pthread_mutex_t lock;
    int data_size;
    char data[0];
} ll_node_t;

typedef struct hcu_queue_ {
    ll_node_t *head;
    uint16_t queue_size;
    uint16_t rem_queue_size;
} hcu_queue_t;

hcu_queue_t* init_queue(int queue_size);
void insert_to_hcu_queue(char *data, int data_size, hcu_queue_t *queue_p); 
