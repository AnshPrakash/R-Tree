//kind of pseudo code

void read_data(){
}

void Allocate_points(data, FileHandler& fh, std::vector<int> pageIds=std::vector<int>(data.size(),-1){ //, int start_id=-1, int end_id=-1){ depends on how page_id's are arranged
  int num_points = data.size(); //temp
  int num_blocks = num_points/maxCap+1;
  int size = maxCap;
  int data_ctr=0;
  std::vector<int> new_pageIds;
  std::vector< std::vector< int > > new_data;
  for(int i=0;i<num_blocks;i++){
    Node n; //modify Allocate_node
    root_node=n; //global variable
    if(i==num_blocks-1)
      size=num_points%maxCap;
    node.size=size;
    for(int j=0;j<size;j++){
      n.childptr[j]=pageIds[data_ctr++];
      n.childMBR[j]=data[data_ctr++]; //temp, assuming data to be in correct format
    }
    //n.pageId= 
    //new_pageIds.push_back(n.pageId);
    //
    //n.parentId=-1;
    n.MBR= Btree::MinBoundingRegion(n.childMBR,size);
    //new_data.push_back(n.MBR); //use clone??
    DiskWrite(n,fh);
  }
  if(num_blocks/maxCap>0)
    Allocate_points(new_data,fh,new_pageIds);
  
  //should return root_node too
}
