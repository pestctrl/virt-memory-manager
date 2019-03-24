#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unistd.h>
#include <math.h>
#include "everything.h"
using namespace std;

void printTables(Process * p) {
  for(int s = 0; s < p->pageTable.size() * p->segmentTable.size(); s++) {
    //printf("Virtual Address %d equals physical address %d\n", s, convertToPhysical(s, p));
  }
}

Process * getProcess(vector<Process*> processes, int pid) {
  for(int i = 0; i < processes.size(); i++) {
    if(processes[i]->id == pid) {
      return processes[i];
    }
  }
  return NULL;
}

int convertToDecimal(string s) {
  int result = 0;

  for(int i = 2; i < s.length(); i++) {
    result *= 16;
    char c = s.at(i);
    if(c >= 'A' && c <= 'F') {
      result += c - 'A' + 10;
    }
    else {
      result += c - '0';
    }
  }

  return result;
}

Everything * getEverything(char * filename) {
  Everything * e = new Everything;
  ifstream file(filename);

  int numberOfPageFrames, maxSegmentLength, pageSize;
  int pageFramesPerProcess, usedByXAlgorithms, min, max;
  int numberOfProcesses;

  file >> numberOfPageFrames >> maxSegmentLength >> pageSize;
  file >> pageFramesPerProcess >> usedByXAlgorithms >> min >> max;
  file >> numberOfProcesses;

  vector<Process*> processes;

  int currentSegment = 0;

  for(int i = 0; i < numberOfProcesses; i++) {
    int id, pagesOnDisk;
    file >> id >> pagesOnDisk;
    int segmentsOnDisk = ((pagesOnDisk-1) / maxSegmentLength)+1;
 
    Process * p = new Process;
    p->id = id;

    p->offsetBits = (int)(ceil(log2(pageSize)));
    
    p->pageBits = (int)ceil(log2(maxSegmentLength));
    p->segmentBits = (int)ceil(log2(segmentsOnDisk));

    for(int i = 0; i < maxSegmentLength; i++) {
      p->pageTable.push_back(i);
    }

    for(int i = 0; i < segmentsOnDisk; i++) {
      p->segmentTable.push_back(currentSegment);
      currentSegment++;
    }

    //printf("Process %d has %d pages on disk and %d segments\n", p->id, pagesOnDisk, segmentsOnDisk);

    e->processes.push_back(p);
  }

  int pid;
  while(file >> pid) {
    string line; file >> line;

    if(line == "-1") {
      //printf("Process %d is done\n", pid);
    }
    else {
      getProcess(e->processes, pid)->accesses.push_back(convertToDecimal(line));
    }
  }

  for(int i = 0; i < processes.size(); i++) {
    //printf("Process %d has %d instructions\n", processes[i]->id, processes[i]->accesses.size());
  }

  e->pageFramesPerProcess = pageFramesPerProcess;
  e->xValue = usedByXAlgorithms;
  e->wsMin = min;
  e->wsMax = max;

  return e;
}
