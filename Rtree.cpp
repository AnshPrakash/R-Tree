#include <vector>
#include <limits.h>

class Node{
  public:
    int pageId; // page no. in which node resides
    int idx; // idx of node in a page
    std::vector< std::vector< int > > childMBR;
    std::vector< std::vector< int > > childptr;
    bool leaf; // leaf node or internal node
    int n; // no. of children in a node( current size)
    static int d; // dimension of points in R tree
    static int maxCap; // maximum no. of children in a node
    static int M; // maximum no. of nodes in a Page
}
