#include <iostream>
#include <cmath>
#include "ReplacementAlgorithm.h"
using namespace std;

LRUX::LRUX(int pages, int id, int shmid, int semnum, int x) : IReplacementAlgorithm(pages, id, shmid, semnum){
  xVal = x-1;
  queue = new int[xVal];
  incr = 0;
  for(int i = 0; i < xVal; i++) {
    queue[i] = -1;
  }
}

void printQueue(int * arr, Entry * pageFrames, int xVal) {
  for(int i = 0; i < xVal; i++) {
    cout << pageFrames[arr[i]].virtPage << " ";
  }
  cout << endl;
}

void LRUX::access(int virt, Process * p) {
  int virtSegAndPage = virt / (int)pow(2,p->offsetBits);
  int index = getMainMemoryIndex(virtSegAndPage);
  if(index == -1) {
    int slot;

    if(queue[0] == -1 || queue[xVal] == -1) {
      slot = incr;
      incr++;
    }
    else {
      slot = queue[xVal];
    }

    int phys = pageFault(virt, p);
    loadPage(slot, virt, phys, p);

    for(int i = numPages-1; i >= 0; i--) {
      queue[i] = queue[i-1];
    }
  }
  else {
    int slot;
    for(slot = 0; slot < xVal; slot++) {
      if(queue[slot] == index)
	break;
    }

    for(int i = slot; i > 0; i--) {
      queue[i] = queue[i-1];
    }
    accessMainMemory(index, virt);
  }

  queue[0] = index;
  //printQueue(queue,pageFrames,xVal);
}
