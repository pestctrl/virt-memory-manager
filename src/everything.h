#include <vector>
using namespace std;

struct Process {
  int id, pageBits, segmentBits, offsetBits;
  vector<int> pageTable;
  vector<int> segmentTable;
  vector<int> accesses;
};

struct Arguments {
  int pageFramesPerProcess, xValue, wsMin, wsMax;
  vector<Process*> processes;
};

Arguments * parseArguments(char * filename);

int twoToThe(int x);

int convertToPhysical(int virtualAddress, Process * p);
