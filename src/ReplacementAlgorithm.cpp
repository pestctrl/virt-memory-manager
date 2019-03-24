#include <iostream>
#include <cmath>
#include "ReplacementAlgorithm.h"
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

OPTX::OPTX(int pages, int id, int shmid, int semnum, int x, vector<int> future) : IReplacementAlgorithm(pages, id, shmid, semnum){
  xVal = x;
  futures = future;
  current = 0;
}

int * OPTX::calculateDistances(int offbits) {
  int * distances = new int[numPages];
  //cout << "current " << current << endl;
  for(int i = 1; i <= futures.size() && i <= xVal; i++) {
    //cout << "Future " << i << ": " << futures[i+current] / (int)pow(2,p->offsetBits) << endl;
  }
  for(int i = 0; i < numPages; i++) {
    distances[i] = -1;
    int page = pageFrames[i].virtPage;
    //cout << page << " ";

    for(int j = 1; j <= futures.size() && j <= xVal; j++) {
      if(futures[j+current] / (int)pow(2,offbits) == page) {
	//cout << "Address " << futures[j+current] << " is in page " << page << " with distance " << j << endl;
	distances[i] = j;
	break;
      }
    }
  }
  //cout << endl;

  return distances;
}

int OPTX::maximum(int * arr) {
  int max = 0; 
  for(int i = 0; i < numPages; i++) {
    // cout << arr[i] << " ";
    if(pageFrames[i].virtPage == -1) {
      max = i;
    }
    else {
      if(pageFrames[max].virtPage != -1) {
	if(arr[i] == -1)
	  max = i; 
	else if(arr[max] != -1 && arr[max] < arr[i])
	  max = i;
      }
    }
  }
  //cout << endl;
  return max;
}

int OPTX::getFarthestIndex(int offsetBits) {
  int * distances = calculateDistances(offsetBits);
  return maximum(distances);
}

void OPTX::access(int virt, Process * p) {
  int virtSegAndPage = virt / (int)pow(2,p->offsetBits);
  int index = getMainMemoryIndex(virtSegAndPage);
  if(index == -1) {
    int farthest = getFarthestIndex(p->offsetBits);
    int phys = pageFault(virt, p);
    loadPage(farthest,virt,phys,p);
  }
  else {
    accessMainMemory(index,virt);
  }
  current++;
}

LDF::LDF(int pages, int id, int shmid, int semnum, vector<int> accesses, int offsetBits) : IReplacementAlgorithm(pages, id, shmid, semnum) {
  modulo = 0;
  for(int i = 0; i < accesses.size(); i++) {
    int temp = accesses[i]/(int)pow(2,offsetBits);
    if(temp > modulo)
      modulo = temp;
  }
}

int positiveModulo(int i, int n) {
  return (i % n + n) % n;
}

int * LDF::calculateDistances(int page) {
  int * distances = new int[numPages];

  for(int i = 0; i < numPages; i++) {
    int otherPage = pageFrames[i].virtPage;
    if(otherPage == -1) {
      distances[i] = -1;
    }
    else {
      int clockwise = positiveModulo((otherPage - page), modulo);
      int counterClockwise = positiveModulo((page - otherPage), modulo);
      
      if(clockwise < counterClockwise) {
	distances[i] = clockwise;
      }
      else {
	distances[i] = counterClockwise;
      }
    }
    //cout << distances[i] << endl;

    //printf("Distance from %d to %d is %d\n", page, otherPage, distances[i]);
  }
  
  return distances;
}

int maximum(int * distances, int numPages) {
  int max = 0;

  for(int i = 0; i < numPages; i++) {
    if(distances[i] == -1) {
      max = i;
    }
    else {
      if(distances[max] != -1 && distances[max] < distances[i])
	max = i;
    }
  }

  //printf("Max distance is index %d\n", max);
  
  return max;
}

int LDF::getLongestPage(int page) {
  int * distances = calculateDistances(page);
  return maximum(distances, numPages);
}

void LDF::access(int virt, Process * p) {
  int virtSegAndPage = virt / (int)pow(2,p->offsetBits);
  int index = getMainMemoryIndex(virtSegAndPage);
  if(index == -1) {
    int longest = getLongestPage(virtSegAndPage);
    int phys = pageFault(virt, p);
    loadPage(longest,virt,phys,p);
  }
  else {
    accessMainMemory(index,virt);
  }
}

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
