#include <sys/sem.h>
#include "semaphores.h"

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

int getVal(int semid, int semnum) {
  union semun  {
    int val;
    struct semid_ds *buf;
    ushort *array;
  } arg;

  return semctl(semid, semnum, GETVAL, arg);
}
