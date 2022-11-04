#include <stdint.h>
#include <stdlib.h>
#include <math.h>

void SHA(unsigned char Input[] ,int start,int end, unsigned char SHAkey[]) {
// This function generates a 20 element hash key. Each element is 1 Byte, thus, total size of hash is 20*8*1 = 160 Bits
   
   for(int i=0;i<20;i++)
    {
      // This function initialises the hash key to '/'
        SHAkey[i] = '/';
    }
    
  for(int i=start,j=0; (i<end && j<20); i++,j++)
    {
     // This function assigns a simple computation of Input to hash array 
        SHAkey[j] = (Input[i]+4)/2;
   //printf("%c\n",SHAkey[j]);
    }
    return;


}
