#include <iostream>
#include <stdlib.h>
#include<string.h>
#include<stdio.h>
#include <unordered_map>
#include <vector>
#include <wolfssl/wolfcrypt/sha256.h>
#include "../App.h"

using namespace std;


int dedup_hash(char shaSum[],std::unordered_map<std::vector<char>, int, VectorHasher> &shaMap, int curr)
{
   // std::string key;

//    for(int x = 0; x < len; x++)
//     {
//          key[x] = shaSum[x];
//     }
   
    //key = shaSum;

    //printf("\n%s\n", key);
    //std::unordered_map<std::string,int>::const_iterator got = shaMap.find(key);
    std::vector<char> shaSumV(shaSum, shaSum + SHA256_DIGEST_SIZE);
    if(shaMap.find(shaSumV) == shaMap.end())
    {
        shaMap.insert(make_pair(shaSumV,curr));
       	//curr++;
        return -1;

    }
    else
    {   
        int x = shaMap.at(shaSumV);
        return x;
    }    


}