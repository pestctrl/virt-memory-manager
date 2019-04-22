#include <iostream>
#include <cmath>
#ifndef ReplacementAlgorithm
#define ReplacementAlgorithm
#include "ReplacementAlgorithm.h"
#endif
using namespace std;

FIFO::FIFO(int pages, int id, int semid, int semnum) : IReplacementAlgorithm(pages, id, semid, semnum){
  nextSpot = 0;
}

void FIFO::access(int virt, Process * p) {
  int virtSegAndPage = virt / (int)pow(2,p->offsetBits);
  int index = getMainMemoryIndex(virtSegAndPage);
  if(index == -1) {
    int phys = pageFault(virt, p);
    loadPage(nextSpot%numPages, virt, phys, p);
    nextSpot++;
  }
  else
    accessMainMemory(index, virt);
}
