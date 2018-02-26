#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <iostream>

#include "rlcsa_builder.h"
#include "misc/utils.h"

#ifdef MULTITHREAD_SUPPORT
#include <omp.h>
#include <mcstl.h>
#endif


namespace CSA
{


RLCSABuilder::RLCSABuilder(usint _block_size, usint _sample_rate, usint _buffer_size, usint _threads) :
  block_size(_block_size), sample_rate(_sample_rate), buffer_size(_buffer_size),
  threads(_threads),
  buffer(0)
{
  this->reset();
  this->build_time = this->search_time = this->sort_time = this->merge_time = 0.0;
}

RLCSABuilder::~RLCSABuilder()
{
  delete this->index;
  delete[] this->buffer;
}

//--------------------------------------------------------------------------

void
RLCSABuilder::insertSequence(char* sequence, usint length, bool delete_sequence)
{
  if(sequence == 0 || length == 0 || !this->ok)
  {
    if(delete_sequence) { delete[] sequence; }
    return;
  }

  if(this->buffer == 0)
  {
    double start = readTimer();
    RLCSA* temp = new RLCSA((uchar*)sequence, length, this->block_size, this->sample_rate, false, false);
    this->build_time += readTimer() - start;
    this->addRLCSA(temp, (uchar*)sequence, length + 1, delete_sequence);
    return;
  }

  if(this->buffer_size - this->chars > length)
  {
    memcpy(this->buffer + this->chars, sequence, length);
    if(delete_sequence) { delete[] sequence; }
    this->chars += length;
    this->buffer[this->chars] = 0;
    this->chars++;
  }
  else
  {
    if(this->chars > 0)
    {
      this->flush();
      this->buffer = new uchar[this->buffer_size];
    }
    if(length >= this->buffer_size - 1)
    {
      double start = readTimer();
      RLCSA* temp = new RLCSA((uchar*)sequence, length, this->block_size, this->sample_rate, false, false);
      this->build_time += readTimer() - start;
      this->addRLCSA(temp, (uchar*)sequence, length + 1, delete_sequence);
    }
    else
    {
      memcpy(this->buffer + this->chars, sequence, length);
      if(delete_sequence) { delete[] sequence; }
      this->chars += length;
      this->buffer[this->chars] = 0;
      this->chars++;
    }
  }
}

void
RLCSABuilder::insertFromFile(const std::string& base_name)
{
  if(!this->ok) { return; }

  if(this->buffer != 0 && this->chars > 0)
  {
    this->flush();
    this->buffer = new uchar[this->buffer_size];
  }

  std::ifstream input(base_name.c_str(), std::ios_base::binary);
  if(!input) { return; }
  RLCSA* increment = new RLCSA(base_name);
  usint data_size = increment->getSize() + increment->getNumberOfSequences();
  uchar* data = new uchar[data_size];
  input.read((char*)data, data_size);
  input.close();

  this->addRLCSA(increment, data, data_size, true);
}

RLCSA*
RLCSABuilder::getRLCSA()
{
  if(this->chars > 0) { this->flush(); }

  RLCSA* temp = this->index;
  this->reset();

  return temp;
}

char*
RLCSABuilder::getBWT(usint& length)
{
  if(this->chars > 0)
  {
    this->flush();
    if(this->buffer_size > 0) { this->buffer = new uchar[this->buffer_size]; }
  }

  if(this->index == 0 || !(this->ok))
  {
    length = 0;
    return 0;
  }

  length = this->index->getSize() + this->index->getNumberOfSequences();
  return (char*)(this->index->readBWT());
}

bool
RLCSABuilder::isOk()
{
  return this->ok;
}

double
RLCSABuilder::getBuildTime()
{
  return this->build_time;
}

double
RLCSABuilder::getSearchTime()
{
  return this->search_time;
}

double
RLCSABuilder::getSortTime()
{
  return this->sort_time;
}

double
RLCSABuilder::getMergeTime()
{
  return this->merge_time;
}

//--------------------------------------------------------------------------

void
RLCSABuilder::flush()
{
  double start = readTimer();
  RLCSA* temp = new RLCSA(this->buffer, this->chars, this->block_size, this->sample_rate, false, (this->index == 0));
  this->build_time += readTimer() - start;
  this->addRLCSA(temp, this->buffer, this->chars, (this->index != 0));
  this->buffer = 0; this->chars = 0;
}

void
RLCSABuilder::addRLCSA(RLCSA* increment, uchar* sequence, usint length, bool delete_sequence)
{
  if(this->index != 0)
  {
    double start = readTimer();

    usint sequences = increment->getNumberOfSequences();
    usint* end_markers = new usint[sequences];
    usint curr = 0;
    for(usint i = 0; i < length - 1; i++)
    {
      if(sequence[i] == 0) { end_markers[curr++] = i; }
    }
    end_markers[sequences - 1] = length - 1;

    usint* positions = new usint[length]; usint begin;
    #ifdef MULTITHREAD_SUPPORT
    usint chunk = std::max((usint)1, sequences / (8 * this->threads));
    omp_set_num_threads(this->threads);
    #pragma omp parallel private(begin)
    {
      #pragma omp for schedule(dynamic, chunk)
      for(sint i = 0; i < (sint)sequences; i++)
      {
        if(i > 0) { begin = end_markers[i - 1] + 1; } else { begin = 0; }
        this->index->reportPositions(sequence + begin, end_markers[i] - begin, positions + begin);
      }
    }
    #else
    for(sint i = 0; i < (sint)sequences; i++)
    {
      if(i > 0) { begin = end_markers[i - 1] + 1; } else { begin = 0; }
      this->index->reportPositions(sequence + begin, end_markers[i] - begin, positions + begin);
    }
    #endif
    delete[] end_markers;
    if(delete_sequence) { delete[] sequence; }

    double mark = readTimer();
    this->search_time += mark - start;

    #ifdef MULTITHREAD_SUPPORT
    mcstl::SETTINGS::num_threads = this->threads;
    #endif
    std::sort(positions, positions + length);
    for(usint i = 0; i < length; i++)
    {
      positions[i] += i + 1;  // +1 because the insertion will be after positions[i]
    }

    double sort = readTimer();
    this->sort_time += sort - mark;

    RLCSA* merged = new RLCSA(*(this->index), *increment, positions, this->block_size, this->threads);
    delete[] positions;
    delete this->index;
    delete increment;
    this->index = merged;

    this->merge_time += readTimer() - sort;  
  }
  else
  {
    if(delete_sequence) { delete[] sequence; }
    this->index = increment;
  }

  this->ok &= this->index->isOk();
}

void
RLCSABuilder::reset()
{
  this->index = 0;

  if(this->buffer_size != 0)
  {
    this->buffer = new uchar[this->buffer_size];
  }
  this->chars = 0;

  this->ok = true;
}


} // namespace CSA
