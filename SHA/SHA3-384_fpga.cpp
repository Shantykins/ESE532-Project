#include <stdio.h>
#include <string.h>
#include <wolfssl/options.h>
#include <wolfssl/wolfcrypt/sha256.h>
//#include "dedup_hash.cpp"
#include <unordered_map>
#include<iostream>

void SHA_new(char* message, char* digest)
{
    
    wc_Sha256 sha;
    wc_InitSha256(&sha);
    wc_Sha256Update(&sha, (const unsigned char*)message, strlen(message));
    wc_Sha256Final(&sha, (unsigned char*)digest);
    
}