#include "utils.h"

#ifdef MULTITHREAD_SUPPORT
#include <omp.h>
#else
#include <cstdlib>
#include <sys/time.h>
#include <sys/resource.h>
#endif


namespace CSA
{

//--------------------------------------------------------------------------

Triple::Triple() :
  first(0), second(0), third(0)
{
}

Triple::Triple(usint a, usint b, usint c) :
  first(a), second(b), third(c)
{
}

//--------------------------------------------------------------------------

std::streamoff
fileSize(std::ifstream& file)
{
  std::streamoff curr = file.tellg();

  file.seekg(0, std::ios::end);
  std::streamoff size = file.tellg();
  file.seekg(0, std::ios::beg);
  size -= file.tellg();

  file.seekg(curr, std::ios::beg);
  return size;
}

std::streamoff
fileSize(std::ofstream& file)
{
  std::streamoff curr = file.tellp();

  file.seekp(0, std::ios::end);
  std::streamoff size = file.tellp();
  file.seekp(0, std::ios::beg);
  size -= file.tellp();

  file.seekp(curr, std::ios::beg);
  return size;
}

std::ostream&
operator<<(std::ostream& stream, pair_type data)
{
  return stream << "(" << data.first << ", " << data.second << ")";
}

void
readRows(std::ifstream& file, std::vector<std::string>& rows, bool skipEmptyRows)
{
  while(file)
  {
    std::string buf;
    std::getline(file, buf);
    if(skipEmptyRows && buf.length() == 0) { continue; }
    rows.push_back(buf);
  }
}

double
readTimer()
{
  /*#ifdef MULTITHREAD_SUPPORT
  return omp_get_wtime();
  #else
  return clock() / (double)CLOCKS_PER_SEC;
  #endif*/
	double usertime, systime;
	struct rusage usage;
	getrusage (RUSAGE_SELF, &usage);
	usertime = (double) usage.ru_utime.tv_sec + (double) usage.ru_utime.tv_usec / 1000000.0;
	systime = (double) usage.ru_stime.tv_sec + (double) usage.ru_stime.tv_usec / 1000000.0;
	return (usertime + systime);
}


} // namespace CSA
