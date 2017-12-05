#include <vector>
using namespace std;

struct Process {
  int id, pageBits, segmentBits, offsetBits;
  vector<int> instructions;
  vector<int> pageTable;
  vector<int> segmentTable;
  vector<int> accesses;
};
