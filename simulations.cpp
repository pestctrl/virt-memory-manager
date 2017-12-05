#include <iostream>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/sem.h>
#include "simulations.h"
#include "ReplacementAlgorithm.h"
using namespace std;

struct Entry {
  int virtualAddr;
  int physicalAddr;
  int data;
};

void op_sem(int semid, int semnum, int op) {
  struct sembuf sb;
  sb.sem_num = semnum;
  sb.sem_op = op;
  sb.sem_flg = 0;

  semop(semid, &sb, 1);
}

void signal(int semid, int semnum) {
  op_sem(semid, semnum, 1);
}

void wait(int semid, int semnum) {
  op_sem(semid, semnum, -1);
}

void pageFault(Entry &e, int virtualAddress, Process * p, int shmid) {
  int * counter = (int*)shmat(shmid,0,0);
  (*counter)++;
  shmdt(counter);

  e.physicalAddr = convertToPhysical(virtualAddress, p);
  e.data = 2;
}

bool isInMainMemory(int vaddr, Entry * mainMemory, int frames) {
  for(int i = 0; i < frames; i++) {
    if(mainMemory[i].virtualAddr == vaddr)
      return true;
  }
  return false;
}

void becomeProcess(Everything * e, int semid, int shmid, int id) {
  wait(semid, id+1);
  
  Entry * pageFrames = new Entry[e->pageFramesPerProcess];
  int nextSpot = 0;
  int counter = 0;

  IReplacementAlgorithm * ra = NULL;
  ra = new LIFO();
  ra->access(10);

  for(int i = 0; i < e->pageFramesPerProcess; i++) {
    pageFrames[i].virtualAddr = -1;
  }
  
  for(int i = 0; i < e->processes[id]->accesses.size(); i++) {
    int virtAddr = e->processes[id]->accesses[i];
    int virtSegAndPage = virtAddr / twoToThe(e->processes[id]->offsetBits);

    if(!isInMainMemory(virtSegAndPage, pageFrames, e->pageFramesPerProcess)) {
      pageFrames[nextSpot%e->pageFramesPerProcess].virtualAddr = virtSegAndPage;
      pageFault(pageFrames[nextSpot%e->pageFramesPerProcess], virtAddr, e->processes[id], shmid);
      counter++;
      nextSpot++;
    }
  }
  
  printf("Process %d: %d page faults\n", e->processes[id]->id, counter);
  signal(semid, 0);
}

void FIFO(Everything * e) {
  long key; 
  int pageFaultCounter = shmget(key++, sizeof(int), IPC_CREAT | 0666);
  int semid = semget(key++, e->processes.size()+1, IPC_CREAT | 0666);

  int id;
  int numProcesses = e->processes.size();

  for(id = -2; id < numProcesses; id++) {
    if(fork() != 0) {
      break;
    }
  }

  if(id == -2) {
    for(int i = 0; i < numProcesses; i++) {
      signal(semid, i+1);
      wait(semid, 0);
    }
    int * counter = (int*)shmat(pageFaultCounter,0,0);
    printf("Total page faults: %d", *counter);
  }
  else if(id == -1) {
    cout << "I am the page fault handler" << endl;
  }
  else {
    becomeProcess(e, semid, pageFaultCounter, id);
  }

}

void runSimulations(Everything * e) {
  FIFO(e);
}
