#include <stdint.h>
#include <stdlib.h>
#include <math.h>

void toySHA(unsigned char Input[] ,int CHUNK, unsigned char SHAkey[]) {
// This function generates a 20 element hash key. Each element is 1 Byte, thus, total size of hash is 20*8*1 = 160 Bits
   
   for(int i=0;i<20;i++)
    {
      // This function initialises the hash key to '/'
        SHAkey[i] = '/';
    }

  for(int i=0,j=0;i<CHUNK,j<20;i++,j++)
    {
     // This function assigns a simple computation of Input to hash array 
        SHAkey[j] = (Input[i]+4)/2;
   printf("%c\n",SHAkey[j]);
    }
    return;


}

void main() {

  unsigned char Input[60] = "Hi I am Prateek ";
  unsigned char SHAkey[20];
  int CHUNK = 60;
  int stat;

  toySHA(Input, CHUNK, SHAkey);
  dedup(SHAkey, stat);
  // printf("\n");
  // for(int i=0;i<6;i++)
  //   {
  //   printf("%c", SHAkey[i]);
  //   }
  // printf("\n");

  }

