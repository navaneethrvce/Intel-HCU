#include "hcu_driver.h"

extern SHA256_CTX glob_sha_256_ctx;
void main() 
{
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    hcu_sha_256_hash_gen("kruthika", 8, &sha256, true);
}
