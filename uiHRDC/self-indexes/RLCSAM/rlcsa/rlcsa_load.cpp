#include <cstdlib>
#include <ctime>
#include <iostream>
#include <vector>

#include "rlcsa.h"
#include "misc/utils.h"

#ifdef MULTITHREAD_SUPPORT
#include <omp.h>
#endif


using namespace CSA;

int main(int argc, char** argv)
{
  std::cout << "RLCSA test" << std::endl;
  if(argc < 3)
  {
    std::cout << "Usage: rlcsa_test basename sample" << std::endl;
    return 1;
  }

  std::cout << "Base name: " << argv[1] << std::endl;
  std::cout << "Sample: " << argv[2] << std::endl;

    double start = readTimer();
    RLCSA* rlcsa= new RLCSA(argv[1], false,argv[2]);
	double load_time = readTimer() - start;
    rlcsa->printInfo();
    rlcsa->reportSize(true);
    std::cout<<"Time:"<<load_time<<std::endl;
    delete rlcsa;
  return 0;
}
