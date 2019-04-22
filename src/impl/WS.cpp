#include <iostream>
#include <cmath>
#ifndef ReplacementAlgorithm
#define ReplacementAlgorithm
#include "ReplacementAlgorithm.h"
#endif
using namespace std;


WS::WS(int pages, int id, int shmid, int semnum, int min, int max) : IReplacementAlgorithm(pages,id,shmid,semnum) {
}

void WS::access(int virt, Process * p) {
  int virtSegAndPage = virt/(int)pow(2,p->offsetBits);
  int index = getMainMemoryIndex(virtSegAndPage);
    
  if(index == -1) {
    int slot;
    if(workingSet.size() != numPages) {
      for(slot = 0; slot < numPages; slot++) {
	if(pageFrames[slot].virtPage == -1) {
	  break;
	}
      }
    }
    else {
      int slot = getMainMemoryIndex(workingSet[numPages]);
    }
    
    int phys = pageFault(virt, p);
    loadPage(slot,virt,phys,p);
    workingSet.push_back(virtSegAndPage);
    workingSet.erase(workingSet.begin());
  }
  else {
    accessMainMemory(index, virt);
  }
}
