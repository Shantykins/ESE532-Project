#include <stdio.h>
#include <string.h>
#include <wolfssl/options.h>
#include <wolfssl/wolfcrypt/sha.h>
//#include "dedup_hash.cpp"
#include <unordered_map>
#include<iostream>

void SHA_new(char* message, char* digest)
{
    printf("Trying to hash: %s\n",message);
    
    Sha sha;
    wc_InitSha(&sha);
    wc_ShaUpdate(&sha, (const unsigned char*)message, strlen(message));
    wc_ShaFinal(&sha, (unsigned char*)digest);
    
    for(int x = 0; x < SHA_DIGEST_SIZE; x++)
    {
        printf("%x",digest[x]);
    }
    printf("\n");
}