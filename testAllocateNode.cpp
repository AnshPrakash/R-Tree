//Sample file for students to get their code running

#include<iostream>
#include "file_manager.h"
#include "errors.h"
#include<cstring>
#include "Rtree.h"

// int PAGEID;
// int d = 1;
// int maxCap =  (PAGE_CONTENT_SIZE - 8*d - 16)/(8*d+4);

// std::vector< int > AllocateNode(FileHandler& fh,int parentId){
//   PageHandler ph = fh.NewPage();
  
//   // std::cout << "maxCap : "<<maxCap<<"\n";
//   Node n = Node(d,maxCap);
//   n.pageId = ph.GetPageNum();
//   PAGEID = n.pageId;
//   n.parentId = parentId;
//   char *data = ph.GetData();
//   std::vector< int > v;
//   v.push_back(n.pageId);
//   v.push_back(n.parentId);
//   v.insert(v.end(),n.MBR.begin(),n.MBR.end());
//   for(int i = 0; i < maxCap; i++){
//     v.insert(v.end(),n.childMBR[i].begin(),n.childMBR[i].end());
//     v.insert(v.end(),n.childptr[i]);
//   }
//   v.push_back(n.leaf);
//   v.push_back(n.size);
//   // std::cout << maxCap <<"\n";
//   // std::cout << v.size()*sizeof(int)<<"\n";
//   memcpy(&data[0],&v[0],v.size()*sizeof(int));
//   fh.FlushPages();
//   // std::cout << "Data written and flushed" << endl;
//   return v;
// }




// Node GetNode(FileHandler& fh,int Id){
//   PageHandler ph = fh.PageAt(Id);
//   Node n;
//   memcpy(&n,&data[0],sizeof(Node));
// }

// using namespace std;

int main() {
  FileManager fm;

  // Create a brand new file
  FileHandler fh = fm.CreateFile("temp.txt");
  // cout << "File created " << endl;

  Btree T = Btree(10,47,fh);
  Node root = T.DiskRead(T.rootPageId,fh);
  root.parentId = 7;
  T.DiskWrite(root,fh);
  Node t = T.DiskRead(T.rootPageId,fh);
  std::cout << T.Equal(root,t) << "\n";

  // Close the file and destory it
  fm.CloseFile (fh);
  fm.DestroyFile ("temp.txt");
  return 0;
}
