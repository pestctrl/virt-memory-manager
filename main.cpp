#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unistd.h>
#include <math.h>
#include "simulations.h"
using namespace std;

int main(int argc, char ** argv) {
  runSimulations(getEverything(argv[1]));
}
