#include <algorithm>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <vector>

#ifdef MULTITHREAD_SUPPORT
#include <omp.h>
#endif

#include "rlcsa_builder.h"
#include "misc/utils.h"


using namespace CSA;


double indexParts(std::vector<std::string>& filename, usint threads, Parameters& parameters);

const int MAX_THREADS = 64;


int
main(int argc, char** argv)
{
  std::cout << "Parallel RLCSA builder" << std::endl;
  if(argc < 3)
  {
    std::cout << "Usage: parallel_build listname output [threads]" << std::endl;
    return 1;
  }

  std::ifstream filelist(argv[1], std::ios_base::binary);
  if(!filelist)
  {
    std::cerr << "Error opening file list!" << std::endl;
    return 2;
  }
  std::vector<std::string> files;
  readRows(filelist, files, true);
  filelist.close();
  std::cout << "Input files: " << files.size() << std::endl;

  std::string base_name = argv[2];
  std::cout << "Output: " << base_name << std::endl;

  usint threads = 1;
  if(argc > 3)
  {
    threads = std::min(MAX_THREADS, std::max(atoi(argv[3]), 1));
  }
  std::cout << "Threads: " << threads << std::endl; 
  std::cout << std::endl;

  std::string parameters_name = base_name + PARAMETERS_EXTENSION;
  Parameters parameters;
  parameters.set(RLCSA_BLOCK_SIZE);
  parameters.set(SAMPLE_RATE);
  parameters.set(SUPPORT_LOCATE);
  parameters.set(SUPPORT_DISPLAY);
  parameters.read(parameters_name);
  parameters.print();

  double start = readTimer();
  double megabytes = indexParts(files, threads, parameters);

  std::cout << "Phase 2: Merging the indexes" << std::endl;
  RLCSABuilder builder(parameters.get(RLCSA_BLOCK_SIZE), parameters.get(SAMPLE_RATE), 0, threads);
  for(std::vector<std::string>::iterator iter = files.begin(); iter != files.end(); iter++)
  {
    std::cout << "Increment: " << *iter << std::endl;
    builder.insertFromFile(*iter);
  }
  std::cout << std::endl;

  RLCSA* index = builder.getRLCSA();
  if(index != 0 && index->isOk())
  {
    index->printInfo();
    index->reportSize(true);
    index->writeTo(base_name);
    parameters.write(parameters_name);
  }
  delete index;

  double stop = readTimer();
  std::cout << megabytes << " megabytes indexed in " << (stop - start) << " seconds (" << (megabytes / (stop - start)) << " MB/s)." << std::endl;
  std::cout << "Search time:  " << builder.getSearchTime() << " seconds" << std::endl;
  std::cout << "Sort time:    " << builder.getSortTime() << " seconds" << std::endl;
  std::cout << "Merge time:   " << builder.getMergeTime() << " seconds" << std::endl;
  std::cout << std::endl;

  return 0;
}


double
indexParts(std::vector<std::string>& filenames, usint threads, Parameters& parameters)
{
  double start = readTimer();
  std::cout << "Phase 1: Building indexes for input files" << std::endl;
  usint block_size = parameters.get(RLCSA_BLOCK_SIZE);
  usint sample_rate = parameters.get(SAMPLE_RATE);
  usint total_size = 0;

  std::ifstream* input_file;
  usint size;
  std::string parameters_name;
  RLCSA* index;
  uchar* data;
  sint i;

  #ifdef MULTITHREAD_SUPPORT
  omp_set_num_threads(threads);
  #pragma omp parallel private(input_file, size, parameters_name, index, data)
  {
    #pragma omp for schedule(dynamic, 1)
  #endif
    for(i = 0; i < (sint)(filenames.size()); i++)
    {
      #ifdef MULTITHREAD_SUPPORT
      #pragma omp critical
      {
      #endif
        size = 0; data = 0;
        std::cout << "Input: " << filenames[i] << std::endl;
        input_file = new std::ifstream(filenames[i].c_str(), std::ios_base::binary);
        if(input_file == 0)
        {
          std::cerr << "Error opening input file " << filenames[i] << "!" << std::endl;
        }
        else
        {
          size = fileSize(*input_file);
          data = new uchar[size];
          input_file->read((char*)data, size);
          delete input_file;
        }
      #ifdef MULTITHREAD_SUPPORT
      }
      #endif

      if(size > 0)
      {
        index = new RLCSA(data, size, block_size, sample_rate, true, true);
        if(index != 0 && index->isOk()) { index->writeTo(filenames[i]); }
        delete index;

        #ifdef MULTITHREAD_SUPPORT
        #pragma omp critical
        {
        #endif
          total_size += size;
          parameters_name = filenames[i] + PARAMETERS_EXTENSION;
          parameters.write(parameters_name);
        #ifdef MULTITHREAD_SUPPORT
        }
        #endif
      }
      else
      {
        #ifdef MULTITHREAD_SUPPORT
        #pragma omp critical
        {
        #endif
          std::cerr << "Warning: Empty input file " << filenames[i] << "!" << std::endl;
        #ifdef MULTITHREAD_SUPPORT
        }
        #endif
      }
    }
  #ifdef MULTITHREAD_SUPPORT
  }
  #endif

  double stop = readTimer();
  double megabytes = total_size / (double)MEGABYTE;
  std::cout << "Indexed " << megabytes << " megabytes in " << (stop - start) << " seconds (" << (megabytes / (stop - start)) << " MB/s)." << std::endl;
  std::cout << std::endl;

  return megabytes;
}
