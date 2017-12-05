#include <iostream>
#include <unistd.h>
#include <sys/sem.h>
#include <sys/types.h>
using namespace std;

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

  return semop(semid, &sb, 1);
}

int main() {
  long key;
  int semid = semget(key++, 1, IPC_CREAT | 0666);

  if(fork() == 0) {
    cout << "Child always goes first" << endl;
    sleep(1);
    signal(semid,0);
  }
  else {
    int res;
    while((res = trywait(semid,0)) == -1) {
      cout << "Parent trying to wait: " << res << endl;
    }
    cout << "Parent goes next" << endl;
  }
}
