//Sample file for students to get their code running

#include<iostream>
#include "file_manager.h"
#include "errors.h"
#include<cstring>
#include "Rtree.h"


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
