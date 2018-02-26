#include <cstdlib>
#include <fstream>
#include <iostream>
#include <vector>

#include "rlcsa_builder.h"
#include "misc/utils.h"


using namespace CSA;



int
lineByLineRLCSA(std::string base_name, usint block_size, usint sample_rate, usint buffer_size)
{
  Parameters parameters;
  parameters.set(RLCSA_BLOCK_SIZE.first, block_size);
  parameters.set(SAMPLE_RATE.first, sample_rate);

  if(sample_rate > 0)
  {
    parameters.set(SUPPORT_LOCATE.first, 1);
    parameters.set(SUPPORT_DISPLAY.first, 1);
  }
  else
  {
    parameters.set(SUPPORT_LOCATE.first, 0);
    parameters.set(SUPPORT_DISPLAY.first, 0);
  }

  std::string parameters_name = base_name + PARAMETERS_EXTENSION;
  parameters.print();
  parameters.write(parameters_name);

  std::cout << "Input: " << base_name << std::endl;
  std::ifstream input_file(base_name.c_str(), std::ios_base::binary);
  if(!input_file)
  {
    std::cerr << "Error opening input file!" << std::endl;
    return 2;
  }
  std::cout << "Buffer size: " << buffer_size << " MB" << std::endl;
  std::cout << std::endl;

  double start = readTimer();
  RLCSABuilder builder(parameters.get(RLCSA_BLOCK_SIZE), parameters.get(SAMPLE_RATE), buffer_size * MEGABYTE);

  usint lines = 0, total = 0;
  while(input_file)
  {
    char buffer[16384];  // FIXME What if lines are longer? Probably fails.
    input_file.getline(buffer, 16384);
    usint chars = input_file.gcount();
    lines++; total += chars;
    if(chars >= 16383) { std::cout << lines << ": " << chars << " chars read!" << std::endl; }
    if(chars > 1) { builder.insertSequence(buffer, chars - 1, false); }
  }

  RLCSA* rlcsa = 0;
  if(builder.isOk())
  {
    rlcsa = builder.getRLCSA();
    rlcsa->writeTo(base_name);
  }
  else
  {
    std::cerr << "Error: RLCSA construction failed!" << std::endl;
    return 3;
  }

  double time = readTimer() - start;
  double build_time = builder.getBuildTime();
  double search_time = builder.getSearchTime();
  double sort_time = builder.getSortTime();
  double merge_time = builder.getMergeTime();

  double megabytes = rlcsa->getSize() / (double)MEGABYTE;
  usint sequences = rlcsa->getNumberOfSequences();
  std::cout << sequences << " sequences" << std::endl;
  std::cout << megabytes << " megabytes in " << time << " seconds (" << (megabytes / time) << " MB/s)" << std::endl;
  std::cout << "(build " << build_time << " s, search " << search_time << "s, sort " << sort_time << " s, merge " << merge_time << " s)" << std::endl;
  std::cout << std::endl;

  delete rlcsa;
  return 0;
}


int
main(int argc, char** argv)
{
  std::cout << "Line-by-line RLCSA builder" << std::endl;
  if(argc < 3)
  {
    std::cout << "Usage: build_rlcsa base_name buffer_size [block_size [sample_rate]]" << std::endl;
    return 1;
  }
  std::cout << std::endl;

  int name_arg = 1, buffer_arg = 2, block_arg = 3, sample_arg = 4;
  usint block_size = (argc > block_arg ? atoi(argv[block_arg]) : RLCSA_BLOCK_SIZE.second);
  usint sample_rate = (argc > sample_arg ? atoi(argv[sample_arg]) : SAMPLE_RATE.second);
  return lineByLineRLCSA(argv[name_arg], block_size, sample_rate, atoi(argv[buffer_arg]));
}
