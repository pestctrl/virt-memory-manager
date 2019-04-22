#include <iostream>
#include <cmath>
#ifndef ReplacementAlgorithm
#define ReplacementAlgorithm
#include "ReplacementAlgorithm.h"
#endif
using namespace std;

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

