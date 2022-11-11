#include <iostream>
#include <stdlib.h>
#include<string.h>
#include<stdio.h>
#include <unordered_map>

using namespace std;


int dedup_hash(char shaSum[],std::unordered_map<std::string, int> &shaMap, int curr)
{
   // std::string key;

//    for(int x = 0; x < len; x++)
//     {
//          key[x] = shaSum[x];
//     }
   
    //key = shaSum;

    //printf("\n%s\n", key);
    //std::unordered_map<std::string,int>::const_iterator got = shaMap.find(key);
    
    if(shaMap.find(shaSum) == shaMap.end())
    {   printf("\n %d \n", curr);
        shaMap.insert(make_pair(shaSum,curr));
        printf("\n Key not found -> Written to Table");
       	//curr++;
        return -1;

    }
    else
    {   
        printf("\n Found");
        int x = shaMap.at(shaSum);
        return x;
    }    


}
