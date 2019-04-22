#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unistd.h>
#include <math.h>
#include "simulations.h"
using namespace std;

int main(int argc, char ** argv) {
  if(argc < 2) {
    cout << "Need file name" << endl;
    return -1;
  }
  
  bool verbose;
  printf("Would you like to run in verbose mode? (y/n) ");
  char c; cin >> c;
  //cout << "Your response was " << c << endl;
  switch(c) {
  case 'y': verbose = true;
    break;
  case 'n': verbose = false;
    break;
  default: cout << "Your response was invalid. Defaulting to \"n\"" << endl;
    verbose = false;
  }
  runSimulations(parseArguments(argv[1]), verbose);
}
