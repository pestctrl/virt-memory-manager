#include <vector>
using namespace std;

struct Process {
  int id, pageBits, segmentBits, offsetBits;
  vector<int> pageTable;
  vector<int> segmentTable;
  vector<int> accesses;
};

struct Everything {
  int pageFramesPerProcess, xValue, wsMin, wsMax;
  vector<Process*> processes;
};

Everything * getEverything(char * filename);

int twoToThe(int x);

int convertToPhysical(int virtualAddress, Process * p);
