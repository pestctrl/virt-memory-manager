#include "IReplacementAlgorithm.h"
#include <vector>
using namespace std;

class FIFO : public IReplacementAlgorithm {
 public:
  int nextSpot;
  FIFO(int pages, int id, int shmid);
  virtual void access(int virt, int segBits, int pageBits, int offBits);
};

class LIFO : public IReplacementAlgorithm {
 public:
  int max;
  LIFO(int pages, int id, int shmid);
  virtual void access(int virt, int segBits, int pageBits, int offBits);
};

class LRUX : public IReplacementAlgorithm {
 public:
  int incr;
  int xVal;
  int * queue;
  LRUX(int pages, int id, int shmid, int x);
  virtual void access(int virt, int segBits, int pageBits, int offBits);
};

class LDF : public IReplacementAlgorithm {
 public:
  int modulo;
  LDF(int pages, int id, int shmid, vector<int> accesses, int offBits);
  int getLongestPage(int page);
  int * calculateDistances(int page);
  virtual void access(int virt, int segBits, int pageBits, int offBits);
};

class OPTX : public IReplacementAlgorithm {
 public:
  int xVal;
  vector<int> futures;
  int current;
  OPTX(int pages, int id, int shmid, int x, vector<int> accesses);
  int getFarthestIndex(int offBits);
  int maximum(int * arr);
  int * calculateDistances(int offBits);
  virtual void access(int virt, int segBits, int pageBits, int offBits);
};

class WS : public IReplacementAlgorithm {
 public:
  WS(int pages, int id, int shmid, int min, int max);
  virtual void access(int virt, int segBits, int pageBits, int offBits);
};