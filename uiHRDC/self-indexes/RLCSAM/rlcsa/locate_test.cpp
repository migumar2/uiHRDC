#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iostream>

#include "rlcsa.h"
#include "misc/utils.h"


using namespace CSA;


int main(int argc, char** argv)
{
  std::cout << "RLCSA locate test" << std::endl;
  if(argc < 3)
  {
    std::cout << "Usage: locate_test basename [begin end] output [direct]" << std::endl;
    return 1;
  }

  usint begin = 0, end = 0;
  int output_arg = 2;
  std::cout << "Base name: " << argv[1] << std::endl;
  if(argc >= 5)
  {
    output_arg = 4;
    begin = atoi(argv[2]), end = atoi(argv[3]);
    std::cout << "Begin: " << begin << std::endl;
    std::cout << "End: " << end << std::endl;
    if(begin > end)
    {
      std::cerr << "Error: Empty range!" << std::endl;
      return 2;
    }
  }
  if(argc < output_arg + 2)
  {
    std::cout << "Using run-based optimizations." << std::endl;
  }
  else
  {
    std::cout << "Using direct locate." << std::endl;
  }
  std::cout << std::endl;

  RLCSA rlcsa(argv[1]);
  if(!rlcsa.supportsLocate())
  {
    std::cerr << "Error: Locate is not supported!" << std::endl;
    return 3;
  }

  if(argc >= 5)
  {
    if(end >= rlcsa.getSize())
    {
      std::cerr << "Error: Invalid range!" << std::endl;
      return 4;
    }
  }
  else
  {
    begin = 0; end = rlcsa.getSize() - 1;
  }

  std::ofstream output(argv[output_arg], std::ios_base::binary);
  if(!output)
  {
    std::cerr << "Error: Cannot open output file!" << std::endl;
    return 5;
  }

  usint* buffer = new usint[MILLION];
  std::clock_t start = std::clock();
  if(argc < output_arg + 2)  // Use run-based optimizations for locate.
  {
    for(usint curr = begin; curr <= end; curr += MILLION)
    {
      pair_type range(curr, std::min(end, curr + MILLION - 1));
      rlcsa.locate(range, buffer);
      for(usint i = 0; i < range.second + 1 - range.first; i++)
      {
        output.write((char*)&(buffer[i]), sizeof(usint));
      }
    }
  }
  else  // Use direct locate.
  {
    for(usint curr = begin; curr <= end; curr += MILLION)
    {
      pair_type range(curr, std::min(end, curr + MILLION - 1));
      for(usint i = 0; i < range.second + 1 - range.first; i++)
      {
        buffer[i] = rlcsa.locate(curr + i);
      }
      for(usint i = 0; i < range.second + 1 - range.first; i++)
      {
        output.write((char*)&(buffer[i]), sizeof(usint));
      }
    }
  }
  std::clock_t stop = std::clock();
  delete[] buffer;

  double size = (end + 1 - begin);
  double time = (stop - start) / (double)CLOCKS_PER_SEC;
  std::cout << size << " locates in " << time << " seconds (" << (size / time) << " locates/s)" << std::endl;
  output.close();

  return 0;
}
