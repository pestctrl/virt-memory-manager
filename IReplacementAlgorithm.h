#include <iostream>
#include "Entry.h"

class IReplacementAlgorithm {
 public:
  int * pageFrames;
  IReplacementAlgorithm();
  virtual void access(int virt) = 0;
};
