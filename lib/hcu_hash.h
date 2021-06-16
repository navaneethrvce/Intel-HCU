#include <stdbool.h>
#include <stdio.h>
#include <openssl/sha.h>

void hcu_sha_256_hash_gen(char *data, int data_size, SHA256_CTX *sha256_ctx, bool is_commit);
