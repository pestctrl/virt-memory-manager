#include <iostream>
#include <cmath>
#include "LIFO.h"
using namespace std;

LIFO::LIFO(int pages, int id, int shmid) : IReplacementAlgorithm(pages, id, shmid){
  max = 0;
}

void LIFO::access(int virt, int segBits, int pageBits, int offBits) {
  int virtSegAndPage = virt / (int)pow(2,offBits);
  if(!inMainMemory(virtSegAndPage)) {
    pageFrames[max].virtPage = virtSegAndPage;
    pageFault();
    if(max < numPages-1) {
      max++;
    }
  }
}

