#include "IReplacementAlgorithm.h"
#include <sys/shm.h>
#include <cmath>
#include <iostream>
#include <unistd.h>
#include <cstdlib>
using namespace std;

IReplacementAlgorithm::IReplacementAlgorithm(int pages, int pid, int sema, int semanum) {
  id = pid;
  pageFrames = new Entry[pages];
  semid = sema;
  semnum = semanum*2+5;
  for(int i = 0; i < pages; i++) {
    pageFrames[i].virtPage = -1;
  }
  numPages = pages;
  verbose = false;
}

int IReplacementAlgorithm::getMainMemoryIndex(int pageAddr) {
  for(int i = 0; i < numPages; i++) {
    if(pageFrames[i].virtPage == pageAddr) {
      return i;
    }
  }
  return -1;
}

int twoToThe(int x) {
  return (int)pow(2,x);
}

int convertToPhysical(int virtualAddress, Process * p) {
  int offset = virtualAddress % twoToThe(p->offsetBits);
  int virtualPage = virtualAddress / twoToThe(p->offsetBits) % twoToThe(p->pageBits);
  int virtualSegment = virtualAddress / twoToThe(p->offsetBits + p->pageBits);

  int physicalPage = p->pageTable[virtualPage];
  int physicalSegment = p->segmentTable[virtualSegment];

  int physicalAddress = physicalSegment;
  physicalAddress *= twoToThe(p->pageBits);
  physicalAddress += physicalPage;
  physicalAddress *= twoToThe(p->offsetBits);
  physicalAddress += offset;

  return physicalAddress;
}

int IReplacementAlgorithm::pageFault(int address, Process * p) {
  wait(semid,3);
  signal(semid,semnum);
  signal(semid,3);

  int physical = convertToPhysical(address,p);
  if(verbose) {
    printf("Process %d encountered page fault while looking up address %d\n", id, address);
    printf("Physical address is");
    usleep(250000);
    printf(".");
    usleep(250000);
    printf(".");
    usleep(250000);
    printf(".");
    printf("%d\n", physical);
    usleep(250000);
  }
  return physical;
}

void IReplacementAlgorithm::loadPage(int slot, int vaddr, int phys, Process * p) {
  pageFrames[slot].virtPage = vaddr / (int)pow(2,p->offsetBits);
  pageFrames[slot].physPage = phys / (int)pow(2,p->offsetBits);
  pageFrames[slot].data = rand();

  if(verbose) {
    printf("Data found at %d is: %d\n", phys, pageFrames[slot].data);
    printf("Placing pageFrame %d into slot %d on main memory\n\n", phys, slot);
    usleep(500000);
  }
}

void IReplacementAlgorithm::accessMainMemory(int slot, int vaddr) {
  if(verbose) {
    printf("Page access %d was in main memory\n", vaddr);
    printf("Page %d was in slot %d with data %d\n\n", pageFrames[slot].physPage, slot, pageFrames[slot].data);
    usleep(500000);
  }
}
