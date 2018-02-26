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


const int MAX_THREADS = 64;
const usint MAX_OCCURRENCES = 100000;


int main(int argc, char** argv)
{
  std::cout << "RLCSA test" << std::endl;
  if(argc < 2)
  {
    std::cout << "Usage: rlcsa_test basename [patterns [threads]]" << std::endl;
    return 1;
  }

  std::cout << "Base name: " << argv[1] << std::endl;
  if(argc > 2) { std::cout << "Patterns: " << argv[2] << std::endl; }
  sint threads = 1;
  if(argc > 3)
  {
    threads = std::min(MAX_THREADS, std::max(atoi(argv[3]), 1));
  }
  std::cout << "Threads: " << threads << std::endl; 
  std::cout << std::endl;

  const RLCSA rlcsa(argv[1], false);
  rlcsa.printInfo();
  rlcsa.reportSize(true);

  if(argc < 3) { return 0; }
  std::ifstream patterns(argv[2], std::ios_base::binary);
  if(!patterns)
  {
    std::cerr << "Error opening pattern file!" << std::endl;
    return 2;
  }

  std::vector<std::string> rows;
  readRows(patterns, rows, true);

  usint total = 0, total_size = 0, ignored = 0;
  sint i, n = rows.size();
  pair_type result;
  usint* matches;

  double start = readTimer();
  #ifdef MULTITHREAD_SUPPORT
  usint occurrences;
  omp_set_num_threads(threads);
  #pragma omp parallel private(result, matches, occurrences)
  {
    #pragma omp for schedule(dynamic, 1)
    for(i = 0; i < n; i++)
    {
      result = rlcsa.count(rows[i]);
      occurrences = length(result);
      #pragma omp critical
      {
        if(occurrences <= MAX_OCCURRENCES) { total += occurrences; }
        else { ignored++; }
        total_size += rows[i].length();
      }
      if(occurrences <= MAX_OCCURRENCES)
      {
        matches = rlcsa.locate(result);
        delete[] matches;
      }
    }
  }
  #else
  for(i = 0; i < n; i++)
  {
    result = rlcsa.count(rows[i]);
    total += length(result);
    matches = rlcsa.locate(result);
    if(matches) { delete[] matches; }
  }
  #endif

  double time = readTimer() - start;
  std::cout << "Patterns:    " << n << " (" << (n / time) << " / sec)" << std::endl;
  std::cout << "Total size:  " << total_size << " bytes (" << (total_size / time) << " / sec)" << std::endl;
  std::cout << "Matches:     " << total << " (" << (total / time) << " / sec)" << std::endl;
  std::cout << "Time:        " << time << " seconds" << std::endl;
  std::cout << std::endl;
  std::cout << "Ignored " << ignored << " patterns with more than " << MAX_OCCURRENCES << " occurrences." << std::endl;
  std::cout << std::endl;

  return 0;
}
