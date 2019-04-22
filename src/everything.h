#include <vector>

struct Process {
  int id, pageBits, segmentBits, offsetBits;
  std::vector<int> pageTable;
  std::vector<int> segmentTable;
  std::vector<int> accesses;
};

struct Arguments {
  int pageFramesPerProcess, xValue, wsMin, wsMax;
  std::vector<Process*> processes;
};

Arguments * parseArguments(char * filename);

int twoToThe(int x);

int convertToPhysical(int virtualAddress, Process * p);
