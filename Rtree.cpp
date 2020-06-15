#include "Rtree.h"
#include<cstring>
#include <cmath>


Node::Node(int d, int maxCap){
  MBR = std::vector< int >(2*d,INT_MIN);
  childMBR = std::vector< std::vector< int > >(maxCap,std::vector< int >(2*d,INT_MIN));
  childptr = std::vector< int >(maxCap,-1);
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