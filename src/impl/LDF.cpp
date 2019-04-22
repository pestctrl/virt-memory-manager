#include <iostream>
#include <cmath>
#include "ReplacementAlgorithm.h"
using namespace std;

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

