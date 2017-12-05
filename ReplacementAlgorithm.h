#include "IReplacementAlgorithm.h"

class FIFO : public IReplacementAlgorithm{
 public:
  int * memory;
  FIFO();
  virtual void access(int virt);
};

class LIFO : public IReplacementAlgorithm {
 public:
  int * memory;
  LIFO();
  virtual void access(int virt);
};
