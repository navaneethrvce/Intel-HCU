#include "hcu_hash.h"

void main() 
{
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    hcu_sha_256_hash_gen("abcdef", 6, &sha256, true);
}
