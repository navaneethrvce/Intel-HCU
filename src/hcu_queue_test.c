#include<stdio.h>
#include "hcu_queue.h"
void main() 
{
    printf("Hello navaneeth\n");
    hcu_queue_t *test_q = init_queue(10);
    insert_to_hcu_queue("ab", 3, test_q, true, NULL);
    insert_to_hcu_queue("abc",4, test_q, true, NULL);
    insert_to_hcu_queue("abcd",5, test_q, true, NULL);

}
