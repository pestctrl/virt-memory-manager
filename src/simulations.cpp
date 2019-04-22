#include <iostream>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/sem.h>
#include "simulations.h"

static int READ = 0;
static int WRITE = 1;

enum Strategy {fifo, lifo, lrux, ldf, optx, ws};

void pageFault(Entry &e, int virtPages, Process * p, int shmid) {
  int * counter = (int*)shmat(shmid,0,0);
  (*counter)++;
  shmdt(counter);

  e.physPage = convertToPhysical(virtPages, p);
  e.data = 2;
}

bool isInMainMemory(int vaddr, Entry * mainMemory, int frames) {
  for(int i = 0; i < frames; i++) {
    if(mainMemory[i].virtPage == vaddr)
      return true;
  }
  return false;
}

void becomeProcess(Arguments * e, int semid, int id, Strategy s, bool verbose) {
  wait(semid, id*2+4);

  Process * p = e->processes[id];
  
  // int nextSpot = 0;
  // int counter = 0;

  IReplacementAlgorithm * ra = NULL;
  switch(s) {
  case fifo : ra = new FIFO(e->pageFramesPerProcess, p->id, semid, id);
    break;
  case lifo : ra = new LIFO(e->pageFramesPerProcess, p->id, semid, id);
    break;
  case lrux : ra = new LRUX(e->pageFramesPerProcess, p->id, semid, id, e->xValue);
    break;
  case ldf :  ra = new LDF(e->pageFramesPerProcess, p->id, semid, id, p->accesses, p->offsetBits);
    break;
  case optx : ra = new OPTX(e->pageFramesPerProcess, p->id, semid, id, e->xValue, p->accesses);
    break;
  case ws :   ra = new WS(e->pageFramesPerProcess, p->id, semid, id, e->wsMin, e->wsMax);
    break;
  }
  
  ra->verbose = verbose;

  for(int i = 0; i < p->accesses.size(); i++) {
    ra->access(p->accesses[i], p);
  }

  signal(semid, 0);
}

void becomePageHandler(int numProcesses, int semid, Arguments * e) {
  int * faults = new int[numProcesses];
  for(int i = 0; i < numProcesses; i++) {
    faults[i] = 0;
  }
  signal(semid,3);
  while(true) {
    wait(semid,3);
    bool hit = false;
    for(int i = 0; i < numProcesses; i++) {
      if(trywait(semid,i*2+4+1) != -1) {
        hit = true;
        faults[i]++;
      }
    }
    signal(semid,3);
    if(!hit && trywait(semid,2) != -1) {
      int counter = 0;

      for(int i = 0; i < numProcesses; i++) {
        counter += faults[i];
        printf("Process %d has %d faults\n", e->processes[i]->id, faults[i]);
      }
	
      printf("Total page faults: %d\n", counter);
      signal(semid,0);
      exit(0);
    }
  }
}

void runSimulation(Arguments * e, Strategy s, bool verbose) {
  switch(s) {
  case fifo : std::cout << "Running FIFO simulation" << std::endl;
    break;
  case lifo : std::cout << "Running LIFO simulation" << std::endl;
    break;
  case lrux : std::cout << "Running LRUX simulation" << std::endl;
    break;
  case optx : std::cout << "Running OPTX simulation" << std::endl;
    break;
  case ldf :  std::cout << "Running LDF simulation" << std::endl;
    break;
  case ws :   std::cout << "Running WS simulation" << std::endl;
    break;
  }
  
  long key = 0; 
  int semid = semget(key++, e->processes.size()*2+4, IPC_CREAT | 0666);
  int id;
  int numProcesses = e->processes.size();

  for(id = -2; id < numProcesses-1; id++) {
    if(fork() != 0) {
      break;
    }
  }

  if(id == -2) {
    for(int i = 0; i < numProcesses; i++) {
      signal(semid, i*2+4);
      wait(semid, 0);
    }
    signal(semid,2);
    wait(semid,0);
    std::cout << std::endl;
  }
  else if(id == -1) {
    becomePageHandler(numProcesses, semid,e);
    // It is not evident that the process will exit
    // However, exit is embedded in a loop,
    // which makes me unsure of what I can do 
    // exactly. Needs to be refactored.
  }
  else {
    becomeProcess(e, semid, id, s, verbose);
    exit(0);
  }
}

void runSimulations(Arguments * e, bool verbose) {
  runSimulation(e, fifo, verbose);
  runSimulation(e, lifo, verbose);
  runSimulation(e, lrux, verbose);
  runSimulation(e, ldf, verbose);
  runSimulation(e, optx, verbose);
  runSimulation(e, ws, verbose);
}
