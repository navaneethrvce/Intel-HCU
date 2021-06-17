#include "hcu_hash.h"
#include <string.h>

extern SHA256_CTX glob_sha_256_ctx;
void hcu_sha_256_hash_gen(char *data, int data_size, SHA256_CTX *sha256_ctx, bool is_commit)
{
    printf("hash data received %s with commit %d \n",data, is_commit);
    unsigned char hash[SHA256_DIGEST_LENGTH];
    //SHA256_CTX sha256_ctx_1;
    //SHA256_Init(&sha256_ctx_1);
    SHA256_Update(&glob_sha_256_ctx, data, data_size );
    if(is_commit) {
        SHA256_Final(hash, &glob_sha_256_ctx);
        printf("0x");
        for(int n =0; n < SHA256_DIGEST_LENGTH; n++) {
            printf("%02x", hash[n]);
        }
        printf("\n");
    }
}
