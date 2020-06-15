#ifndef RTREE_H
#define RTREE_H

#include <limits.h>
#include <vector>
#include "file_manager.h"


class Node{
  public:
    int pageId;                 // page no. in which node resides
    int parentId;               // page no. of parent node
    std::vector< int > MBR;
    std::vector< std::vector< int > > childMBR;
    std::vector< int > childptr;
    bool leaf;                  // leaf node or internal node
    int size;                   // no. of children in a node( current size)
    Node(int d,int maxCap);
    Node(){return;}
};


class Btree{
  int d;        // dimension of points in R tree
  int maxCap;   // maximum no. of children in a node
  int M;        // maximum no. of nodes in a Page
  int rootPageId;
  int height;
  int noOfElement;
  Btree(int,int);
  Node DiskRead(int id);            // read the page corresponding to the node to disk
  Node Diskwrite(Node& n);          // write the page corresponding to the node to disk
  Node AllocateNode(FileHandler&,int parentid);  // Allocate page for the node
};

#endif
