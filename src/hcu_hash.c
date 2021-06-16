#include "hcu_hash.h"
void hcu_sha_256_hash_gen(char *data, int data_size, SHA256_CTX *sha256_ctx, bool is_commit)
{
    unsigned char hash[SHA256_DIGEST_LENGTH];
    if(! is_commit) {
        SHA256_Update(sha256_ctx, data, data_size);
    } else {
        SHA256_Final(hash, sha256_ctx);
        printf("%s",hash);
    }
}
