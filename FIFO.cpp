#include <iostream>
#include <cmath>
#include "FIFO.h"
using namespace std;

FIFO::FIFO(int pages, int id, int shmid) : IReplacementAlgorithm(pages, id, shmid){
  nextSpot = 0;
}

void FIFO::access(int virt, int segBits, int pageBits, int offBits) {
  int virtSegAndPage = virt / (int)pow(2,offBits);
  if(!inMainMemory(virtSegAndPage)) {
    pageFrames[nextSpot%numPages].virtPage = virtSegAndPage;
    pageFault();
    nextSpot++;
  }
}

