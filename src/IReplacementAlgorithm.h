#include <iostream>
#include "everything.h"
#include "semaphores.h"

struct Entry {
  int virtPage;
  int physPage;
  int data;
};

class IReplacementAlgorithm {
 public:
  int id;
  int numPages;
  int semid;
  int semnum;
  bool verbose;
  Entry * pageFrames;
  IReplacementAlgorithm(int pages, int id, int semid, int semanum);
  int getMainMemoryIndex(int pageAddr);
  int pageFault(int addr, Process * p);
  void loadPage(int slot, int vaddr, int paddr, Process * p);
  virtual void access(int virt, Process * p) = 0;
  void accessMainMemory(int slot, int vaddr);
};
