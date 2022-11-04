#include <stdint.h>
#include <stdlib.h>
#include <math.h>

static int elements =0;
void toySHA(unsigned char Input[] ,int CHUNK, unsigned char SHAkey[]) {
     // This function generates a 20 element hash key. Each element is 1 byte, thus, 20*1*8 = 160 BIT hash is generated
   // Initialises the hash key to '/' .
   for(int i=0;i<20;i++)
    {
        SHAkey[i] = '/';
 //  printf("%c\n",SHAkey[i]);
    }

   // Assigns a simplified compute of Input to hash array
  for(int i=0,j=0;i<CHUNK,j<20;i++,j++)
    {
        SHAkey[j] = (Input[i]+4)/2;
   printf("%c\n",SHAkey[j]);
    }
    return;


}


void main() {

  unsigned char Input[60] = "Hi I am Shantanu ";
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

