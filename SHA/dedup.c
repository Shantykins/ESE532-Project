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