#include <iostream>
#include "ReplacementAlgorithm.h"
using namespace std;

FIFO::FIFO() {

}

void FIFO::access(int virt) {
  printf("FIFO accessing address %d\n", virt);
}

LIFO::LIFO() {

}

void LIFO::access(int virt) {
  printf("LIFO accessing address %d\n", virt);
}
