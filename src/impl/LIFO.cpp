#include <iostream>
#include <cmath>
#ifndef ReplacementAlgorithm
#define ReplacementAlgorithm
#include "ReplacementAlgorithm.h"
#endif
using namespace std;

LIFO::LIFO(int pages, int id, int shmid, int semnum) : IReplacementAlgorithm(pages, id, shmid, semnum){
  max = 0;
}

void LIFO::access(int virt, Process * p) {
  int virtSegAndPage = virt / (int)pow(2,p->offsetBits);
  int index = getMainMemoryIndex(virtSegAndPage);
  if(index == -1) {
    pageFrames[max].virtPage = virtSegAndPage;
    int phys = pageFault(virt, p);
    loadPage(max,virt,phys,p);
    if(max < numPages-1) {
      max++;
    }
  }
  else 
    accessMainMemory(index, virt);
}

