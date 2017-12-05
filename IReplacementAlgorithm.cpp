#include "IReplacementAlgorithm.h"
#include <sys/shm.h>
#include <iostream>
using namespace std;

IReplacementAlgorithm::IReplacementAlgorithm(int pages, int pid, int shmid) {
  id = pid;
  pageFaults = 0;
  totalPageFaults = shmid;
  pageFrames = new Entry[pages];
  for(int i = 0; i < pages; i++) {
    pageFrames[i].virtPage = -1;
  }
  numPages = pages;
}

int IReplacementAlgorithm::getMainMemoryIndex(int pageAddr) {
  for(int i = 0; i < numPages; i++) {
    if(pageFrames[i].virtPage == pageAddr) {
      return i;
    }
  }
  return -1;
}

void IReplacementAlgorithm::pageFault() {
  pageFaults++;
  int * counter = (int*)shmat(totalPageFaults,0,0);
  (*counter)++;
  shmdt(counter);
}

void IReplacementAlgorithm::report() {
  printf("Process %d has %d page faults\n", id, pageFaults);
}
