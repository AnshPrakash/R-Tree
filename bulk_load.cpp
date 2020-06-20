#include "Rtree.h"


void Btree::bulk_load(FileHandler& fh_1, FileHandler& fh, int N){
  PageHandler ph = fh_1.FirstPage ();
  int pts_per_page = PAGE_CONTENT_SIZE/(sizeof(int)*d);
  int pts_last_page = N%pts_per_page;
  int num_pages = 1 + ((N - 1) / pts_per_page); //ceil quotient
  int cur_page = ph.GetPageNum(); 
  int read_num = 0;
  std::vector< std::vector<int >> ret; //= std::vector< std::vector< int > >(N,std::vector< int >(2*d,INT_MIN));
  
  for(int i=0; i<num_pages; i++){ //or while(true){
    try{
      int num_ctr=0; //which loc in page is being read
      char *data = ph.GetData ();
      //Assuming each point contains one int & last page contains rest of the vectors
      
      if(i==num_pages-1)
        pts_per_page = pts_last_page;
      
      //is there an alternative way of reading pts?
      
      for(int j=0;j<pts_per_page;j++){
        std::vector< int > push_pt = std::vector< int >(2*d,INT_MIN);
        for(int k=0;k<d;k++){
          memcpy (&read_num, &data[num_ctr++], sizeof(int));
          push_pt.push_back(read_num);
          push_pt.push_back(read_num);
        }
        ret.push_back(push_pt);          
      }
      fh_1.FlushPage(cur_page);
      ph = fh_1.NextPage(cur_page);
      cur_page= ph.GetPageNum();
    }
    catch(...){
      continue; //if intermediate page contains <max pts
    }
  }
  rootPageId=Allocate_points(ret, fh, N, true, std::vector<int>(N,-1));
}

int Btree::Allocate_points(std::vector< std::vector<int >> data, FileHandler& fh, int N, bool is_leaf, std::vector<int> pageIds){
  height+=1;
  int num_blocks = 1 + ((N - 1) / maxCap);
  int size = maxCap;
  int data_ctr=0;
  std::vector<int> new_pageIds;
  std::vector< std::vector< int > > new_data;
  for(int i = 0;i < num_blocks;i++){
    Node n = AllocateNode(fh,-1);
    rootPageId=n.pageId; //update every time
    if(i == num_blocks-1)
      size = N%maxCap;
    n.size = size;
    n.leaf= is_leaf;
    for(int j = 0;j < size;j++){
      n.childptr[j] = pageIds[data_ctr];
      n.childMBR[j] = data[data_ctr++];
      if(!is_leaf){
        Node ch = DiskRead(n.childptr[j],fh);
        ch.parentId = n.pageId;
      }
    }
    n.MBR= MinBoundingRegion(n.childMBR,size);
    new_pageIds.push_back(n.pageId);
    new_data.push_back(n.MBR);
    DiskWrite(n,fh);
    FreeNode(n,fh);
  }
  if(num_blocks > 1)
    return Allocate_points(new_data,fh,num_blocks,false,new_pageIds);
  else  
    return rootPageId;
  //should return root_node too
}

