#ifndef UTILS_H
#define UTILS_H

#include <fstream>
#include <vector>

#include "definitions.h"


namespace CSA
{



struct Triple
{
  usint first;
  usint second;
  usint third;

  Triple();
  Triple(usint a, usint b, usint c);
};


std::streamoff fileSize(std::ifstream& file);
std::streamoff fileSize(std::ofstream& file);


std::ostream& operator<<(std::ostream& stream, pair_type data);


void readRows(std::ifstream& file, std::vector<std::string>& rows, bool skipEmptyRows);

double readTimer();


} // namespace CSA


#endif // UTILS_H
