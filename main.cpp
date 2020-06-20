#include <iostream>
#include <fstream>
#include <cstdlib>
#include "Rtree.h"


int main(int argc, char const *argv[]){
  std::ifstream inp;
  std::ofstream  out;
  if( argc < 5) std::cout << "Give correct parameters";
  FileManager fm;
  FileHandler fh = fm.CreateFile("RTREE_INDEX.txt");
  inp.open (argv[1]);
  out.open(argv[4]);
  int maxCap = atoi(argv[2]);
  int dimensionality = atoi(argv[3]);
  Btree rt = Btree(dimensionality,maxCap,fh);
  std::string line;
  while(inp >> line){
    if(line == "BULKLOAD"){
      std::string bulkFile;
      inp >> bulkFile;
      int numpoints;
      inp >> numpoints;
      // FileHandler fh1 = fm.OpenFile(bulkFile.c_str());
      // rt.bulk_load(fh1,fh,numpoints);
      // fm.CloseFile(fh1);
    }
    else if( line == "INSERT"){
      std::vector< int > p(2*dimensionality);
      for(int i = 0; i < dimensionality ; i++){
        inp >> p[2*i];
        p[2*i + 1] = p[2*i];
      }
      // rt.Insert(p,fh);
    }
    else if( line == "QUERY"){
      std::vector< int > p(2*dimensionality);
      for(int i = 0; i < dimensionality ; i++){
        inp >> p[2*i];
        p[2*i + 1] = p[2*i];
      }
      if (rt.Search(p,rt.rootPageId,fh)) out << "TRUE\n\n";
      else out << "FALSE\n\n";
    }
    else  std::cerr <<"Input file incorrect\n";
    // if(line == "BULKLOAD") std::cout << "true\n\";
    // out << line<<"\n";
  }
  inp.close();
  out.close();
  fm.CloseFile(fh);
  fm.DestroyFile("RTREE_INDEX.txt");
  return 0;
}