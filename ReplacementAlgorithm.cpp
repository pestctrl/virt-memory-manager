#include <iostream>
#include <cmath>
#include "ReplacementAlgorithm.h"
using namespace std;

FIFO::FIFO(int pages, int id, int shmid) : IReplacementAlgorithm(pages, id, shmid){
  nextSpot = 0;
}

void FIFO::access(int virt, int segBits, int pageBits, int offBits) {
  int virtSegAndPage = virt / (int)pow(2,offBits);
  if(getMainMemoryIndex(virtSegAndPage) == -1) {
    pageFrames[nextSpot%numPages].virtPage = virtSegAndPage;
    pageFault();
    nextSpot++;
  }
}

LIFO::LIFO(int pages, int id, int shmid) : IReplacementAlgorithm(pages, id, shmid){
  max = 0;
}

void LIFO::access(int virt, int segBits, int pageBits, int offBits) {
  int virtSegAndPage = virt / (int)pow(2,offBits);
  if(getMainMemoryIndex(virtSegAndPage) == -1) {
    pageFrames[max].virtPage = virtSegAndPage;
    pageFault();
    if(max < numPages-1) {
      max++;
    }
  }
}

LRUX::LRUX(int pages, int id, int shmid, int x) : IReplacementAlgorithm(pages, id, shmid){
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

void LRUX::access(int virt, int segBits, int pageBits, int offBits) {
  int virtSegAndPage = virt / (int)pow(2,offBits);
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

    pageFrames[slot].virtPage = virtSegAndPage;
    pageFault();

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
  }

  queue[0] = index;
  //printQueue(queue,pageFrames,xVal);
}

OPTX::OPTX(int pages, int id, int shmid, int x, vector<int> future) : IReplacementAlgorithm(pages, id, shmid){
  xVal = x;
  futures = future;
  current = 0;
}

int * OPTX::calculateDistances(int offBits) {
  int * distances = new int[numPages];
  //cout << "current " << current << endl;
  for(int i = 1; i <= futures.size() && i <= xVal; i++) {
    //cout << "Future " << i << ": " << futures[i+current] / (int)pow(2,offBits) << endl;
  }
  for(int i = 0; i < numPages; i++) {
    distances[i] = -1;
    int page = pageFrames[i].virtPage;
    //cout << page << " ";

    for(int j = 1; j <= futures.size() && j <= xVal; j++) {
      if(futures[j+current] / (int)pow(2,offBits) == page) {
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

int OPTX::getFarthestIndex(int offBits) {
  int * distances = calculateDistances(offBits);
  return maximum(distances);
}

void OPTX::access(int virt, int segBits, int pageBits, int offBits) {
  int virtSegAndPage = virt / (int)pow(2,offBits);
  if(getMainMemoryIndex(virtSegAndPage) == -1) {
    int farthest = getFarthestIndex(offBits);
    pageFrames[farthest].virtPage = virtSegAndPage;
    pageFault();
  }
  current++;
}

LDF::LDF(int pages, int id, int shmid, vector<int> accesses, int offBits) : IReplacementAlgorithm(pages, id, shmid) {
  modulo = 0;
  for(int i = 0; i < accesses.size(); i++) {
    int temp = accesses[i]/(int)pow(2,offBits);
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

void LDF::access(int virt, int segBits, int pageBits, int offBits) {
  int virtSegAndPage = virt / (int)pow(2,offBits);
  if(getMainMemoryIndex(virtSegAndPage) == -1) {
    int longest = getLongestPage(virtSegAndPage);
    pageFrames[longest].virtPage = virtSegAndPage;
    pageFault();
  }
}
