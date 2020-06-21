#ifndef RTREE_H
#define RTREE_H

#include <limits>
#include <vector>
#include "file_manager.h"
#include "errors.h"
#include <string>
#include <cstring>

#define INT_MIN std::numeric_limits<int>::min()
#define INT_MAX std::numeric_limits<int>::max()

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
public:
  int d;        // dimension of points in R tree
  int maxCap;   // maximum no. of children in a node
  int m;        // minimum no. of children in a node
  int M;        // maximum no. of nodes in a Page
  int rootPageId;
  int height;
  int noOfElement;
  Btree(int dim, int maxChildren, FileHandler& fh);
  Node DiskRead(int id,FileHandler& fh);              // read the page corresponding to the node to disk
  Node DiskWrite(Node& n,FileHandler& fh);            // write the page corresponding to the node to disk
  Node AllocateNode(FileHandler&,int parentid);       // Allocate page for the node
  bool Equal(Node& n1,Node& n2);                      //check if two Node are equal or not just for debugging purpose
  bool DeleteNode(Node& n, FileHandler& fh);          // delete the node from disk
  bool FreeNode(const Node& n,FileHandler& fh);              // free node from memory still remains on disk
  void SplitChild(int k,Node& n,FileHandler& fh);     // split kth child of node
  // Todo
  void Insert(const std::vector< int >& p, FileHandler& fh);
  void InsertNonFull(const std::vector< int >& p, Node& n, FileHandler& fh);
  std::vector< Node > QuadraticSplit(const Node& n, FileHandler& fh);  // split a node into two and return the nodes as vector
  bool Search(const std::vector< int >& p, int nodeid, FileHandler& fh);
  void bulk_load(FileHandler& fh_1, FileHandler& fh, int N);
  int Allocate_points(std::vector< std::vector<int >> data, FileHandler& fh, int N, bool is_leaf, std::vector<int> pageIds);
  //helper functions
  // return the index of the MBRs which expands the least when p is included in it
  int LeastIncreasingMBR( const std::vector< int >& p ,const std::vector< std::vector< int > >& possMBRs, int nsize);
  std::vector< int > seed(const Node& n);             // seed the QudraticSplit Algo
  bool contains(const std::vector< int >& p, const std::vector< int >& MBR);        // check if MBR contains p
  double VolMBR( const std::vector< int >&);                                //volume of single MBR
  double VolMBRS( const std::vector< std::vector<int >>& MBRs, int nsize); // sum of volume of all MBRs
  double DeadSpace( int nsize, const std::vector< std::vector<int >>& Elist , const std::vector< int >& MBR);// wasted space in MBR containing E list MBRs
  std::vector< int > MinBoundingRegion(const std::vector< std::vector<int >>& Elist, int nsize);    //  minimum bounding region of a list of MBR
};


#endif
