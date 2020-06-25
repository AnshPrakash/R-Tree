#include "Rtree.h"


void Btree::bulk_load(FileHandler& fh_1, FileHandler& fh, int N){
  int startPageId,lastPageId;
  PageHandler ph = fh_1.FirstPage ();
  int pts_per_page = PAGE_CONTENT_SIZE/(sizeof(int)*d);
  int pts_last_page = N%pts_per_page;
  int num_pages = (int)ceil((N*1.0)/pts_per_page); //1 + ((N - 1) / pts_per_page); //ceil quotient
  int cur_page = ph.GetPageNum();
  startPageId = cur_page;
  int read_num = 0;
  int rem_pts = N;
  for(int i = 0; i < num_pages; i++){ 
    int num_ctr = 0; //which loc in page is being read
    char *data = ph.GetData();
    //Assuming each point contains one int & last page contains rest of the vectors
    
    if(i == num_pages-1 && N%pts_per_page != 0) //change
      pts_per_page = pts_last_page;
    
    std::vector< std::vector<int >> chmbrs = std::vector< std::vector< int > >(maxCap,std::vector< int >(2*d,INT_MIN));
    int child_count = 0;
    for(int j = 0; j < pts_per_page; j++){
      std::vector< int > push_pt; //change
      for(int k = 0; k < d; k++){
        memcpy(&read_num, &data[num_ctr],sizeof(int));
        num_ctr += sizeof(int);
        push_pt.push_back(read_num);
        push_pt.push_back(read_num);
      }
      rem_pts--;
      chmbrs[child_count++] = push_pt;
      if(child_count == maxCap || rem_pts == 0){
        Node n = AllocateNode(fh,-1);
        n.MBR = MinBoundingRegion(chmbrs,child_count);
        n.childMBR = chmbrs;
        n.leaf = true;
        n.size = child_count;
        DiskWrite(n,fh);
        chmbrs = std::vector< std::vector< int > >(maxCap,std::vector< int >(2*d,INT_MIN));
        child_count = 0;
        lastPageId = n.pageId;
        // PrintNode(n);
      }
    }
    fh_1.UnpinPage(cur_page);
    fh_1.FlushPage(cur_page);
    if( i != num_pages -1){
      ph = fh_1.NextPage(cur_page);
      cur_page= ph.GetPageNum();
    }
  
  }
  height = 0;
  fh_1.FlushPages();
  // std::cout << startPageId <<" " << lastPageId <<"\n";
  AssignParents(startPageId,lastPageId,fh);
}

void Btree::AssignParents(int startPageId,int lastPageId, FileHandler& fh){
  if( startPageId == lastPageId){
    rootPageId = startPageId;
    return;
  }

  int start,last;
  Node n = AllocateNode(fh,-1);
  start = n.pageId;
  last = n.pageId;
  // Pages are stored contiguous
  int id = startPageId;
  while(id <= lastPageId){
    for(int i = 0; i < maxCap && id <= lastPageId; i++){
      Node c = DiskRead(id,fh);
      c.parentId = n.pageId;
      n.childptr[i] = c.pageId;
      n.childMBR[i] = c.MBR;
      n.leaf = false;
      n.size += 1;
      id++;
      DiskWrite(c,fh);
    }
    n.MBR = MinBoundingRegion(n.childMBR,n.size);
    DiskWrite(n,fh);
    if( id <= lastPageId){
      n = AllocateNode(fh,-1);
      last = n.pageId;
    }
  }
  height += 1;
  AssignParents(start,last,fh);
}

