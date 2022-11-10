
static int elements = 0; // This has to be global variable to keep track of number of elements in Table. Can be passed as a argument and instead be written in App

int dedup(unsigned char SHAkey[]){

  unsigned char record[100][20];
 int flag=0, same=0;


  if(elements == 0 )
  {
      // This Loop Runs for the case if Table is empty
    for(int i=0;i<1;i++)
    {
    for(int j=0;j<20;j++)
    {
      record[i][j] = SHAkey[j];
      //printf("%c\n",record[i][j]);
    }
    }

    elements++;
  
  }
  
else{

  for(int i=0;i<elements;i++)
  {
    same=0;
    flag=0;
    for(int j=0;j<20;j++)
    {
      //Checks for the similar element
      if(record[i][j] == SHAkey[j])
      {
        same++;
      }
    }
    //If all 20 element match is found, the loop is broken and index should be sent to output.
    if(same==19)
    {
      flag = 1;
      return i;
    }
  }

  // If no match is found, the hash key is added to the table
  if(flag==0){
    for(int j=0;j<20;j++)
      {
        record[elements][j] == SHAkey[j];
      }
    elements++;
    return -1;
  }


}


}
