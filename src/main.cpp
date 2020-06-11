#include "ncurses_display.h"
#include "system.h"
#include <dirent.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <string>
#include <algorithm>
#include <cerrno>
#include <chrono>
#include <cstring>
#include <fstream>
#include <iostream>
#include <iterator>
#include <sstream>
#include <stdexcept>
#include <string>
#include <thread>
#include <vector>
#include "linux_parser.h"
using std::string;
using std::vector;

const string s{"/"};

class test
{
private:
  int x;
  /* data */
public:
  test(/* args */);
  ~test
();
};

test::test(/* args */)
{
  std::cout << "Constructor" << std::endl;
}

test::~test()
{
  std::cout << "De-Constructor" << std::endl;
}

int main() {
  // System system;
  // NCursesDisplay::Display(system);
  std::cout << LinuxParser::MemoryUtilization()<< std::endl;
 
}