#include <iostream>
#include "Entry.h"

class IReplacementAlgorithm {
 public:
  int id;
  int numPages;
  int pageFaults;
  int totalPageFaults;
  Entry * pageFrames;
  IReplacementAlgorithm(int pages, int id, int shmid);
  int getMainMemoryIndex(int pageAddr);
  void pageFault();
  void report();
  virtual void access(int virt, int segBits, int pageBits, int offBits) = 0;
};
