
void op_sem(int semid, int semnum, int op);

void signal(int semid, int semnum);

void wait(int semid, int semnum);

int trywait(int semid, int semnum);

int getVal(int semid, int semnum);
