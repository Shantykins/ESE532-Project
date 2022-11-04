#include <stdint.h>
#include <stdlib.h>
#include <math.h>

static int elements =0;
void toySHA(unsigned char Input[] ,int CHUNK, unsigned char SHAkey[]) {

   for(int i=0;i<20;i++)
    {
        SHAkey[i] = '/';
 //  printf("%c\n",SHAkey[i]);
    }

  for(int i=0,j=0;i<CHUNK,j<20;i++,j++)
    {
        SHAkey[j] = (Input[i]+4)/2;
   printf("%c\n",SHAkey[j]);
    }
    return;


}

void dedup(unsigned char SHAkey[], int stat){

  unsigned char record[20][20];
 int flag=0, same=0;
 stat = 0;
  if(elements ==0 ){
    for(int i=0;i<1;i++){
    for(int j=0;j<20;j++){
      record[i][j] = SHAkey[j];
      printf("%c\n",record[i][j]);
  }
}
    stat = 1;
    elements++;
}
else{

  for(int i=0;i<elements;i++){
    same=0 ;
    flag=0;
    for(int j=0;j<20;j++){
      if(record[i][j] == SHAkey[j])
      {
        same++;
      }
    }
    if(same==6)
    {
      stat = 1;
      flag =1;
    }
  }

  if(flag==0){
    for(int j=0;j<20;j++)
      {
        record[elements][j] == SHAkey[j];
      }
  }


}
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

