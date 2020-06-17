#include "Rtree.h"
#include<cstring>
#include <cmath>


Node::Node(int d, int maxCap){
  MBR = std::vector< int >(2*d,INT_MIN);
  childMBR = std::vector< std::vector< int > >(maxCap,std::vector< int >(2*d,INT_MIN));
  childptr = std::vector< int >(maxCap,-1);
  size = 0;
}


Btree::Btree(int dim, int maxChildren, FileHandler& fh){
  d = dim;
  maxCap =  (PAGE_CONTENT_SIZE - 8*d - 16)/(8*d+4);
  maxCap = std::min(maxChildren,maxCap); //  fit in a page
  m = (int)ceil(maxCap/2.0);
  height = 0;
  Node root = AllocateNode(fh,-1);
  rootPageId = root.pageId;
  noOfElement = 4 + 2*d + (2*d + 1)*maxCap;
  root.leaf = true;
  root.size = 0;
}

Node Btree::AllocateNode(FileHandler& fh, int parentId ){
  PageHandler ph = fh.NewPage();
  Node n = Node(d,maxCap);
  n.pageId = ph.GetPageNum();
  n.parentId = parentId;
  return n;
}

Node Btree::DiskWrite(Node& n,FileHandler& fh){
  PageHandler ph = fh.PageAt(n.pageId);// going to disk or buffer check?
  char *data = ph.GetData();
  std::vector< int > v;
  v.push_back(n.pageId);
  v.push_back(n.parentId);
  v.insert(v.end(),n.MBR.begin(),n.MBR.end());
  for(int i = 0; i < maxCap; i++){
    v.insert(v.end(),n.childMBR[i].begin(),n.childMBR[i].end());
    v.insert(v.end(),n.childptr[i]);
  }
  v.push_back(n.leaf);
  v.push_back(n.size);
  memcpy(&data[0],&v[0],v.size()*sizeof(int));
  fh.MarkDirty(n.pageId);
  fh.UnpinPage(n.pageId);
  fh.FlushPage(n.pageId);
  return n;
}

Node Btree::DiskRead(int id,FileHandler& fh){
  PageHandler ph = fh.PageAt(id);
  char *data = ph.GetData();
  std::vector< int > v(noOfElement);
  memcpy(&v[0],&data[0],noOfElement*sizeof(int));
  Node n = Node(d,maxCap);
  n.pageId = v[0];
  n.parentId = v[1];
  memcpy(&n.MBR[0],&v[2],2*d*sizeof(int));
  int idx = 2 + 2*d;
  for(int i = 0; i < maxCap; i++){
    memcpy(&n.childMBR[i][0],&v[idx],2*d*sizeof(int));
    idx = idx + 2*d;
    memcpy(&n.childptr[i],&v[idx],sizeof(int));
    idx = idx + 1;
  }
  memcpy(&n.leaf,&v[idx],sizeof(int));
  idx += 1;
  memcpy(&n.size,&v[idx],sizeof(int));
  return n;
}

bool Btree::Equal(Node& n1,Node& n2){
  if( n1.pageId != n2.pageId) return false;
  if( n1.parentId != n2.parentId ) return false;
  if(n1.MBR.size() != n2.MBR.size()) return false;
  for(int i = 0; i < n1.MBR.size();i++) if(n1.MBR[i] != n2.MBR[i]) return false;
  if(n1.childptr.size() != n2.childptr.size()) return false;
  for(int i = 0; i < maxCap; i++){
    if(n1.childMBR[i].size() != n2.childMBR[i].size() ) return false;
    if(n1.childptr[i] != n2.childptr[i]) return false;
    for(int j = 0; j < n1.childMBR[i].size(); j++) if(n1.childMBR[i][j] != n2.childMBR[i][j]) return false;
  }
  if(n1.leaf != n2.leaf) return false;
  if(n1.size != n2.size) return false;
  return true;
}


bool Btree::DeleteNode(Node& n, FileHandler& fh){
  return (fh.DisposePage(n.pageId));
}

bool Btree::contains(const std::vector< int >& p, const std::vector< int >& MBR){
  for(int i = 0; i < d; i++){
    if( MBR[2*i] > p[2*i] || MBR[i*2 + 1] < p[2*i + 1] ) return false;
  }
  return true;
}

int Btree::VolMBR( const std::vector< int >& MBR){
  int vol = 1;
  for(int i = 0; i < d; i++) vol = vol*(MBR[2*i + 1] - MBR[2*i]);
  return vol;
}

int Btree::VolMBRS( const std::vector< std::vector<int >>& MBRs, int nsize){
  int vol = 0;
  for( int i = 0 ; i < nsize; i++) vol += VolMBR(MBRs[i]);
  return vol;
}

int Btree::DeadSpace(int nsize ,const std::vector< std::vector<int >>& Elist , const std::vector< int >& MBR){
  int v1 = VolMBRS(Elist,nsize);
  int v2 = VolMBR(MBR);
  return ( v2 - v1);
}

std::vector< int > Btree::MinBoundingRegion(const std::vector< std::vector<int >>& Elist, int nsize){
  std::vector< int > mbr(2*d);
  for(int i = 0; i < d; i++){
    mbr[2*i] = INT_MAX;
    mbr[2*i + 1] = INT_MIN;
    for(int e = 0; e < nsize; e++){
      mbr[2*i] = std::min(Elist[e][2*i], mbr[2*i]);
      mbr[2*i + 1] = std::max(Elist[e][2*i+1], mbr[2*i + 1]);
    }
  }
  return mbr;
}



bool Btree::FreeNode(const Node& n,FileHandler& fh){
  return (fh.UnpinPage(n.pageId));
}


int Btree::LeastIncreasingMBR( const std::vector< int >& p ,const std::vector< std::vector< int > >& possMBRs,int nsize ){
  int minInc = INT_MAX;
  int idx = -1;
  for( int i = 0; i < nsize; i++){
    int inc = VolMBR( MinBoundingRegion({p,possMBRs[i]},2) )  - VolMBR(possMBRs[i]);
    if( minInc > inc ){
      minInc = inc;
      idx = i;
    }
  }
  return idx;
}


void Btree::Insert(const std::vector< int >& p, FileHandler& fh){
  Node r = DiskRead(rootPageId,fh);
  if( r.size == maxCap ){
    Node s = AllocateNode(fh,-1);
    s.leaf = false;
    s.size = 1;
    s.childptr[0] = r.pageId;
    r.parentId = s.pageId;
    s.MBR = r.MBR;
    SplitChild(0,s,fh);
    FreeNode(r,fh); // since it has been updated on disk by SplitChild
    rootPageId = s.pageId;
    height += 1;
    InsertNonFull(p,s,fh);
  }
  else InsertNonFull(p,r,fh);
}




void Btree::InsertNonFull(const std::vector< int >& p, Node& n, FileHandler& fh){
  if( !n.leaf ){
    for( int i = 0; i < n.size; i++){
      if (contains(p,n.childMBR[i])){
        Node ch = DiskRead(n.childptr[i],fh);
        if(ch.size == maxCap) {
          SplitChild(i,n,fh);
          FreeNode(ch,fh); // ch is deleted on the disk by SplitChild, deleting the old copy
          InsertNonFull(p,n,fh);
          return;
        }
        FreeNode(n,fh);
        InsertNonFull(p,ch,fh);
        return;
      }
    }
    int idx = LeastIncreasingMBR(p,n.childMBR,n.size);
    Node ch = DiskRead(n.childptr[idx],fh);
    if(ch.size == maxCap) {
      SplitChild(idx,n,fh);
      FreeNode(ch,fh); // ch is deleted on the disk by SplitChild, deleting the old copy
      InsertNonFull(p,n,fh);
    }
    n.childMBR[idx] = MinBoundingRegion({p,n.childMBR[idx]},2);
    n.MBR = MinBoundingRegion( n.childMBR, n.size);
    DiskWrite(n,fh);
    InsertNonFull(p,ch,fh);
  }
  else{
    // We can simply Add to leaf,since the n.size is always < maxCap 
    // as we always recur to Nodes which are nonfull
    n.childMBR[n.size] = p;
    n.size += 1;
    n.MBR = MinBoundingRegion(n.childMBR,n.size);
    
  }
}



void Btree::SplitChild(int k,Node& n,FileHandler& fh){
  int id = n.childptr[k];
  Node ch = DiskRead(id,fh);
  std::vector< Node > div = QuadraticSplit(ch,fh);
  Node n1,n2;
  n1 = div[0], n2 = div[1];
  n.childptr[k] = n1.pageId;
  n.childMBR[k] = n1.MBR;
  n.childptr[n.size] = n2.pageId;
  n.childMBR[n.size] = n2.MBR;
  n.size += 1;
  DeleteNode(ch,fh);
  DiskWrite(n,fh);
  DiskWrite(n1,fh);
  DiskWrite(n2,fh);
}


std::vector< int > Btree::seed(const Node& n){
  int maxdiff = -1;
  int e1,e2;
  e1 = e2 = -1;
  for(int i = 0; i < n.size; i++){
    for(int j = i + 1; j < n.size; j++){
      int val = DeadSpace(2,{n.childMBR[i],n.childMBR[i]},MinBoundingRegion({n.childMBR[i],n.childMBR[i]},2));
      if( maxdiff < val ){
        maxdiff = val ;
        e1 = i;
        e2 = j;
      }
    }
  }
  return(std::vector<int>({e1,e2}));
}

std::vector< Node > Btree::QuadraticSplit(const Node& n, FileHandler& fh){
  int e1,e2;
  std::vector<int > seedv = seed(n);
  e1 = seedv[0],e2 = seedv[1];
  std::vector< int > L1,L2;
  L1.push_back(e1);L2.push_back(e2);
  std::vector< int > E;
  for(int i = 0; i < n.size; i++) if ( i != e1 || i != e2) E.push_back(i);
  std::vector<std::vector< int >> mbrL1,mbrL2;
  mbrL1.push_back(n.childMBR[e1]);
  mbrL2.push_back(n.childMBR[e2]);
  while( !E.empty() ){
    int maxdiff = -1;
    int idx = -1;
    std::vector<std::vector< int >> tp1,tp2;
    for(auto i: E){
      int d1,d2;
      std::vector<std::vector< int >> temp1,temp2;
      temp1 = mbrL1;temp2 = mbrL2;
      temp1.push_back(n.childMBR[i]);
      temp2.push_back(n.childMBR[i]);
      d1 = DeadSpace(temp1.size(),temp1, MinBoundingRegion(temp1,temp1.size()));
      d2 = DeadSpace(temp2.size(),temp2, MinBoundingRegion(temp2,temp2.size()));;
      if( std::abs(d1 - d2) > maxdiff){
        idx = i;
        maxdiff = std::abs(d1 - d2);
        tp1 = temp1;
        tp2 = temp2;
      }
    }
    int d1 = VolMBRS({MinBoundingRegion(tp1,tp1.size())},1);
    d1 -= VolMBRS(mbrL1,mbrL1.size());
    int d2 = VolMBRS({MinBoundingRegion(tp2,tp2.size())},1);
    d2 -= VolMBRS(mbrL2,mbrL2.size());
    if( d1 < d2 ){
      mbrL1 = tp1;
      L1.push_back(idx);
    }
    else{
      mbrL2 = tp2;
      L2.push_back(idx); 
    }
    E.erase(E.begin() + idx);
  }
  Node n1,n2;
  n1 = AllocateNode(fh,n.parentId);
  n2 = AllocateNode(fh,n.parentId);
  for( auto i : L1){
    n1.childMBR[n1.size] = n.childMBR[i];
    n1.childptr[n1.size] = n.childptr[i];
    n1.size += 1;
  }
  for( auto i : L2){
    n2.childMBR[n2.size] = n.childMBR[i];
    n2.childptr[n2.size] = n.childptr[i];
    n2.size += 1;
  }
  n1.leaf = n.leaf;
  n2.leaf = n.leaf;
  n1.MBR = MinBoundingRegion(n1.childMBR,n1.size);
  n2.MBR = MinBoundingRegion(n2.childMBR,n2.size);
  return (std::vector< Node >({n1,n2}));
}



bool Btree::Search(const std::vector< int >& p,int nodeid, FileHandler& fh){
  Node n = DiskRead(nodeid,fh);
  bool find = false;
  for(int i = 0; i < n.size && !find; i++){
    if( contains(p,n.childMBR[i]) ){
      int childId = n.childptr[i];
      if(!n.leaf) find = find || Search(p,childId,fh);
      else find = find || true;
    }
  }
  FreeNode(n,fh);
  return find;
}