#include "../IReplacementAlgorithm.h"
#include <vector>

class FIFO : public IReplacementAlgorithm {
 public:
  int nextSpot;
  FIFO(int pages, int id, int semid, int semnum);
  virtual void access(int virt, Process * p);
};

class LIFO : public IReplacementAlgorithm {
 public:
  int max;
  LIFO(int pages, int id, int semid, int semnum);
  virtual void access(int virt, Process * p);
};

class LRUX : public IReplacementAlgorithm {
 public:
  int incr;
  int xVal;
  int * queue;
  LRUX(int pages, int id, int shmid, int semnum, int x);
  virtual void access(int virt, Process * p);
};

class LDF : public IReplacementAlgorithm {
 public:
  int modulo;
  LDF(int pages, int id, int shmid, int semnum, std::vector<int> accesses, int offBits);
  int getLongestPage(int page);
  int * calculateDistances(int page);
  virtual void access(int virt, Process * p);
};

class OPTX : public IReplacementAlgorithm {
 public:
  int xVal;
  std::vector<int> futures;
  int current;
  OPTX(int pages, int id, int shmid, int semnum, int x, std::vector<int> accesses);
  int getFarthestIndex(int offBits);
  int maximum(int * arr);
  int * calculateDistances(int offBits);
  virtual void access(int virt, Process * p);
};

class WS : public IReplacementAlgorithm {
 public:
  std::vector<int> workingSet;
  WS(int pages, int id, int shmid, int semnum, int min, int max);
  virtual void access(int virt, Process * p);
};
