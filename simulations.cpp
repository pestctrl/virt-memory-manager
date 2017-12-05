#include <iostream>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/sem.h>
#include "simulations.h"
#include "ReplacementAlgorithm.h"
using namespace std;

enum Strategy {fifo, lifo, lrux, ldf, optx, ws};

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

int trywait(int semid, int semnum) {
  struct sembuf sb;
  sb.sem_num = semnum;
  sb.sem_op = -1;
  sb.sem_flg = IPC_NOWAIT;

  semop(semid, &sb, 1);
}

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

void becomeProcess(Everything * e, int semid, int shmid, int id, Strategy s) {
  wait(semid, id+3);
  Process * p = e->processes[id];
  
  int nextSpot = 0;
  int counter = 0;

  IReplacementAlgorithm * ra = NULL;
  switch(s) {
  case fifo : ra = new FIFO(e->pageFramesPerProcess, p->id, shmid);
    break;
  case lifo : ra = new LIFO(e->pageFramesPerProcess, p->id, shmid);
    break;
  case lrux : ra = new LRUX(e->pageFramesPerProcess, p->id, shmid, e->xValue);
    break;
  case ldf : ra = new LDF(e->pageFramesPerProcess, p->id, shmid, p->accesses, p->offsetBits);
    break;
  case optx : ra = new OPTX(e->pageFramesPerProcess, p->id, shmid, e->xValue, p->accesses);
    break;
    /*
  case ws : ra = new WS(e->pageFramesPerProcess, p->id, shmid, e->min, e->max);
  break;*/
  }

  for(int i = 0; i < p->accesses.size(); i++) {
    ra->access(p->accesses[i], p->segmentBits, p->pageBits, p->offsetBits);
  }

  ra->report();
  
  signal(semid, 0);
}

void runSimulation(Everything * e, Strategy s) {
  switch(s) {
  case fifo : cout << "Running FIFO simulation" << endl;
    break;
  case lifo : cout << "Running LIFO simulation" << endl;
    break;
  case lrux : cout << "Running LRUX simulation" << endl;
    break;
  case optx : cout << "Running OPTX simulation" << endl;
    break;
  case ldf : cout << "Running LDF simulation" << endl;
    break;
  case Strategy::ws : cout << "Running WS simulation" << endl;
    break;
  }
  
  long key; 
  int pageFaultCounter = shmget(key++, sizeof(int), IPC_CREAT | 0666);
  int * c = (int*)shmat(pageFaultCounter,0,0);
  *c = 0;
  shmdt(c);
  int semid = semget(key++, e->processes.size()+3, IPC_CREAT | 0666);

  int id;
  int numProcesses = e->processes.size();

  for(id = -2; id < numProcesses; id++) {
    if(fork() != 0) {
      break;
    }
  }

  if(id == -2) {
    for(int i = 0; i < numProcesses; i++) {
      signal(semid, i+3);
      wait(semid, 0);
    }
    signal(semid,2);
    wait(semid,0);
    cout << endl;
  }
  else if(id == -1) {
    while(true) {
      if(trywait(semid,1) == -1) {

      }
      if(trywait(semid,2) != -1) {
	int * counter = (int*)shmat(pageFaultCounter,0,0);
	printf("Total page faults: %d\n", *counter);
	shmdt(counter);
	signal(semid,0);
	exit(0);
      }
    }
  }
  else {
    becomeProcess(e, semid, pageFaultCounter, id, s);
    exit(0);
  }
}

void runSimulations(Everything * e) {
  runSimulation(e, fifo);
  runSimulation(e, lifo);
  runSimulation(e, lrux);
  runSimulation(e, ldf);
  runSimulation(e, optx);
  //runSimulation(e, ws);
}
