#include<stdlib.h>
#include<stdio.h>
//#include "dedup_hash.cpp"
//#include "SHA3-384_fpga.cpp"
#include<iostream>
#include <wolfssl/options.h>
#include <wolfssl/wolfcrypt/sha3.h>

/*void hardhash(char* message, char* digest)
{
    wc_Sha3 sha3_384;
    wc_InitSha3_384(&sha3_384, NULL, INVALID_DEVID);
    wc_Sha3_384_Update(&sha3_384, (const unsigned char*)message, strlen(message));
    wc_Sha3_384_Final(&sha3_384, (unsigned char*)digest);

    for(int x = 0; x < SHA3_384_DIGEST_SIZE; x++)
    {
        printf("%x",digest[x]);
    }
    printf("\n");
}
*/
int main()
{
      char digest[SHA_DIGEST_SIZE];
    	char message1[] = "Hello There";
      char message2[] = "Prateek Bashista";
      char message4[] = "Moksh Bashista";
	    char message3[] = "Prateek Bashista";

      std::unordered_map<std::string, int> shaMap;
     int curr=0; 
	   int index;
         
    	//hardhash(message1,digest);
      //hardhash(message2,digest);
      //hardhash(message3,digest);
      
     // return 0;
      SHA_new(message1,digest);
      index = dedup_hash(digest,shaMap,curr);
      printf("\n %d \n", curr);
      if(index == -1)
      {
    	printf("\n Chunk Added to Table\n");
      curr++;
    	}
    	
      else
      {
    	printf("\n Chunk found at %d\n", index);
      }
      
      
       
	    SHA_new(message2,digest);
       index = dedup_hash(digest,shaMap,curr);
      printf("\n %d \n", curr);
      if(index == -1)
      {
    	printf("\n Chunk Added to Table\n");
      curr++;
    	}
    	
      else
      {
    	printf("\n Chunk found at %d\n", index);
      }
      
	    SHA_new(message3,digest);
       index = dedup_hash(digest,shaMap,curr);
      printf("\n %d \n", curr);
      if(index == -1)
      {
    	printf("\n Chunk Added to Table\n");
      curr++;
    	}
    	
      else
      {
    	printf("\n Chunk found at %d\n", index);
      }
      
      SHA_new(message4,digest);  	
      index = dedup_hash(digest,shaMap,curr);
      printf("\n %d \n", curr);
      if(index == -1)
      {
    	printf("\n Chunk Added to Table\n");
      curr++;
    	}
    	
      else
      {
    	printf("\n Chunk found at %d\n", index);
      }
      
      return 0;
}
