#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <list>

#include "rlcsa.h"
#include "misc/utils.h"
#include "qsufsort/qsufsort.h"

#ifdef MULTITHREAD_SUPPORT
#include <omp.h>
#endif



namespace CSA
{


RLCSA::RLCSA(const std::string& base_name, bool print,char* ssample) :
  ok(false),
  sa_samples1(0),sa_samples2(0),sa_samples3(0),
  sa_samples4(0),sa_samples5(0),sa_samples6(0),sa_samples7(0),
  end_points(0)
{
  for(usint c = 0; c < CHARS; c++) { this->array[c] = 0; }

  std::string array_name = base_name + ARRAY_EXTENSION;
  std::ifstream array_file(array_name.c_str(), std::ios_base::binary);
  if(!array_file)
  {
    std::cerr << "RLCSA: Error opening Psi array file!" << std::endl;
    return;
  }

  usint distribution[CHARS];
  array_file.read((char*)distribution, CHARS * sizeof(usint));
  this->buildCharIndexes(distribution);

  Parameters parameters;
  parameters.read(base_name + PARAMETERS_EXTENSION);
  for(usint c = 0; c < CHARS; c++)
  {
    if(!isEmpty(this->index_ranges[c])) { this->array[c] = new RLEVector(array_file); }
  }

  this->end_points = new DeltaVector(array_file);
  this->number_of_sequences = this->end_points->getNumberOfItems();

  array_file.read((char*)&(this->sample_rate), sizeof(this->sample_rate));
  this->sample_rate = atoi(ssample);
  array_file.close();

  this->support_locate = parameters.get(SUPPORT_LOCATE);
  this->support_display = parameters.get(SUPPORT_DISPLAY);

  if(this->support_locate || this->support_display)
  {
    std::string sa_sample_name = base_name + "." + ssample + SA_SAMPLES_EXTENSION;
    std::ifstream sa_sample_file(sa_sample_name.c_str(), std::ios_base::binary);

    if(!sa_sample_file)
    {
      std::cerr << "RLCSA: Error opening suffix array sample file!" << std::endl;
      return;
    }
    this->sa_samples1 = new SASamples(sa_sample_file, this->sample_rate);
    sa_sample_file.close();
  }

  if(print) { parameters.print(); }

  this->ok = true;
}

RLCSA::RLCSA(uchar* data, usint bytes, usint block_size, usint sa_sample_rate, bool multiple_sequences, bool delete_data) :
  ok(false),
  sa_samples1(0),sa_samples2(0),sa_samples3(0),
  sa_samples4(0),sa_samples5(0),sa_samples6(0),sa_samples7(0),
  sample_rate(sa_sample_rate),
  end_points(0)
{
  std::cerr<<"RLCSA"<<std::endl;
  for(usint c = 0; c < CHARS; c++) { this->array[c] = 0; }

  if(!data || bytes == 0)
  {
    std::cerr << "RLCSA: No input data given!" << std::endl;
    if(delete_data) { delete[] data; }
    return;
  }
  if(block_size < 2 * sizeof(usint) || block_size % sizeof(usint) != 0)
  {
    std::cerr << "RLCSA: Block size must be a multiple of " << sizeof(usint) << " bytes!" << std::endl;
    if(delete_data) { delete[] data; }
    return;
  }


  // Do we store SA samples?
  if(this->sample_rate > 0)
  {
    this->support_locate = this->support_display = true;
  }
  else
  {
    this->support_locate = this->support_display = false;
    this->sample_rate = 1;
  }


  // Determine the number of sequences and mark their end points.
  if(multiple_sequences)
  {
    DeltaEncoder endings(RLCSA::ENDPOINT_BLOCK_SIZE);
    this->number_of_sequences = 0;
    usint marker = 0;
    usint padding = 0, chars_encountered = 0;

    for(usint i = 0; i < bytes; i++)
    {
      if(data[i] == 0)
      {
        if(i == marker) { break; }  // Empty sequence.
        this->number_of_sequences++;
        marker = i + 1;

        usint pos = chars_encountered + padding - 1;
        endings.setBit(pos);
        padding = ((pos + this->sample_rate) / this->sample_rate) * this->sample_rate - chars_encountered;
      }
      else
      {
        chars_encountered++;
      }
    }

    if(this->number_of_sequences == 0 || marker != bytes)
    {
      std::cerr << "RLCSA: Collection must consist of 0-terminated nonempty sequences!" << std::endl;
      if(delete_data) { delete[] data; }
      return;
    }
    this->end_points = new DeltaVector(endings, chars_encountered + padding);
  }
  else
  {
    this->number_of_sequences = 1;
    DeltaEncoder endings(RLCSA::ENDPOINT_BLOCK_SIZE, RLCSA::ENDPOINT_BLOCK_SIZE);
    usint pos = bytes - 1;
    endings.setBit(pos);
    usint padding = ((pos + this->sample_rate) / this->sample_rate) * this->sample_rate - bytes;
    std::cerr<<"Allocating DeltaVector "<<bytes+padding<<std::endl;
    this->end_points = new DeltaVector(endings, bytes + padding);
    std::cout<<"Delta Size: "<<this->end_points->reportSize()<<std::endl;   
  }


  // Build character tables etc.
  usint distribution[CHARS];
  sint low = CHARS, high = 0;
  for(usint c = 0; c < CHARS; c++) { distribution[c] = 0; }
  for(usint i = 0; i < bytes; i++)
  {
    if(data[i] < low) { low = data[i]; }
    if(data[i] > high) { high = data[i]; }
    distribution[(usint)data[i]]++;
  }
  if(multiple_sequences)
  {
    distribution[0] = 0;
    low = 0;
    high = high + this->number_of_sequences;
  }
  this->buildCharIndexes(distribution);


  std::cerr<<"Allocating first array bytes: "<<bytes+1<<std::endl;
  // Build suffix array.
  int* inverse = new int[bytes + 1];
  if(multiple_sequences)
  {
    int zeros = 0;
    for(usint i = 0; i < bytes; i++)
    {
      if(data[i] == 0)
      {
        inverse[i] = zeros;
        zeros++;
      }
      else
      {
        inverse[i] = (int)(data[i] + this->number_of_sequences);
      }
    }
  }
  else
  {
    for(usint i = 0; i < bytes; i++) { inverse[i] = (int)data[i]; }
  }
  if(delete_data) { delete[] data; }
  std::cerr<<"Allocating second array"<<std::endl;
  int* sa = new int[bytes + 1];
  suffixsort(inverse, sa, bytes, high + 1, low);


  // Sample SA.
  if(this->support_locate || this->support_display)
  {
    if(multiple_sequences)
    {
      this->sa_samples1 = new SASamples((uint*)inverse, this->end_points, this->data_size, this->sample_rate);
    }
    else
    {
std::cerr<<"Builiding samples " <<std::endl;

      this->sa_samples1 = new SASamples((uint*)(sa + 1), this->data_size, 64);
      this->sa_samples2 = new SASamples((uint*)(sa + 1), this->data_size, 128);
      this->sa_samples3 = new SASamples((uint*)(sa + 1), this->data_size, 256);
      this->sa_samples4 = new SASamples((uint*)(sa + 1), this->data_size, 512);
      this->sa_samples5 = new SASamples((uint*)(sa + 1), this->data_size, 32);
      this->sa_samples6 = new SASamples((uint*)(sa + 1), this->data_size, 1024);
      this->sa_samples7 = new SASamples((uint*)(sa + 1), this->data_size, 2048);

std::cerr<<"Samples BUILT "<< std::endl;
    }
  }


  // Build Psi.
  for(usint i = 0; i <= bytes; i++)
  {
    sa[i] = inverse[(sa[i] + 1) % (bytes + 1)];
  }
  delete[] inverse;


  // Build RLCSA.
  usint incr = (multiple_sequences ? this->number_of_sequences + 1 : 1);  // No e_of_s markers in SA.
  usint decr = (multiple_sequences ? 1 : 0);  // Ignore the last e_of_s marker if multiple sequences.
  for(usint c = 0; c < CHARS; c++)
  {
    if(distribution[c] == 0) { this->array[c] = 0; continue; }

    uint* curr = (uint*)(sa + index_ranges[c].first + incr);
    uint* limit = (uint*)(sa + index_ranges[c].second + incr + 1);
    RLEEncoder encoder(block_size);
    pair_type run(*curr, 1);
    curr++;

    for(; curr < limit; curr++)
    {
      if(*curr == run.first + run.second) { run.second++; }
      else
      {
        encoder.setRun(run.first - decr, run.second);
        run = pair_type(*curr, 1);
      }
    }
    encoder.setRun(run.first - decr, run.second);

    this->array[c] = new RLEVector(encoder, this->data_size + incr - decr);
  }
  delete[] sa;

std::cerr<<"RLCSA BUILT "<< std::endl;


  this->ok = true;
}

RLCSA::RLCSA(RLCSA& index, RLCSA& increment, usint* positions, usint block_size, usint threads) :
  ok(false),
  sa_samples1(0),
  end_points(0)
{
  for(usint c = 0; c < CHARS; c++) { this->array[c] = 0; }

  if(!index.isOk() || !increment.isOk())
  {
    return; // Fail silently. Actual error has already been reported.
  }
  if(positions == 0)
  {
    std::cerr << "RLCSA: Positions for insertions not available!" << std::endl;
    return;
  }
  if(index.sample_rate != increment.sample_rate)
  {
    std::cerr << "RLCSA: Cannot combine indexes with different sample rates!" << std::endl;
    return;
  }

  if(index.sa_samples1 == 0 || increment.sa_samples1 == 0)
  {
    this->support_locate = this->support_display = false;
  }
  else
  {
    this->support_locate = this->support_display = true;
  }


  // Build character tables etc.
  usint distribution[CHARS];
  for(usint c = 0; c < CHARS; c++)
  {
    distribution[c] = length(index.index_ranges[c]) + length(increment.index_ranges[c]);
  }
  this->buildCharIndexes(distribution);
  this->sample_rate = index.sample_rate;
  this->number_of_sequences = index.number_of_sequences + increment.number_of_sequences;

  // Merge end points, SA samples, and Psi.
  usint psi_size = this->data_size + this->number_of_sequences;
  bool should_be_ok = true;

  #ifdef MULTITHREAD_SUPPORT
  omp_set_num_threads(threads);
  #pragma omp parallel
  {
    #pragma omp for schedule(dynamic, 1)
  #endif
    for(int c = -2; c < (int)CHARS; c++)
    {
      if(c == -2)      { this->mergeEndPoints(index, increment); }
      else if(c == -1) { this->mergeSamples(index, increment, positions);  }
      else if(distribution[c] != 0)
      {
        this->array[c] = mergeVectors(index.array[c], increment.array[c], positions, increment.data_size + increment.number_of_sequences, psi_size, block_size);
        index.array[c] = 0;
        increment.array[c] = 0;

        if(this->array[c] == 0)
        {
          std::cerr << "RLCSA: Merge failed for vectors " << c << "!" << std::endl;
          should_be_ok = false;
        }
      }
    }
  #ifdef MULTITHREAD_SUPPORT
  }
  #endif

  this->ok = should_be_ok;
}

RLCSA::~RLCSA()
{
  for(usint c = 0; c < CHARS; c++) { delete this->array[c]; }
    if(this->sa_samples1!=NULL)delete this->sa_samples1;
    if(this->sa_samples2!=NULL)delete this->sa_samples2;
    if(this->sa_samples3!=NULL)delete this->sa_samples3;
    if(this->sa_samples4!=NULL)delete this->sa_samples4;
    if(this->sa_samples5!=NULL)delete this->sa_samples5;
    if(this->sa_samples6!=NULL)delete this->sa_samples6;
    if(this->sa_samples7!=NULL)delete this->sa_samples7;
  delete this->end_points;
}

//--------------------------------------------------------------------------

void
RLCSA::writeTo(const std::string& base_name) const
{
  std::string array_name = base_name + ARRAY_EXTENSION;

std::cerr<<"WRITING "<< array_name << std::endl;

  std::ofstream array_file(array_name.c_str(), std::ios_base::binary);
  if(!array_file)
  {
    std::cerr << "RLCSA: Error creating Psi array file!" << std::endl;
    return;
  }

  usint distribution[CHARS];
  for(usint c = 0; c < CHARS; c++)
  {
    distribution[c] = length(this->index_ranges[c]);
  }
  array_file.write((char*)distribution, CHARS * sizeof(usint));
  for(usint c = 0; c < CHARS; c++)
  {
    if(this->array[c] != 0)
    {
      this->array[c]->writeTo(array_file);
    }
  }

  this->end_points->writeTo(array_file);
  array_file.write((char*)&(this->sample_rate), sizeof(this->sample_rate));
  array_file.close();

  if(this->support_locate || this->support_display)
  {
    /*Added by me**/
    std::string sa_sample_name = base_name + ".64" + SA_SAMPLES_EXTENSION;
    std::ofstream sa_sample_file1(sa_sample_name.c_str(), std::ios_base::binary);

    if(!sa_sample_file1)
    {
      std::cerr << "RLCSA: Error creating suffix array sample file!" << std::endl;
      return;
    }
    this->sa_samples1->writeTo(sa_sample_file1);
    sa_sample_file1.close();

    sa_sample_name = base_name + ".128" + SA_SAMPLES_EXTENSION;
    std::ofstream sa_sample_file2(sa_sample_name.c_str(), std::ios_base::binary);
    if(!sa_sample_file2)
    {
      std::cerr << "RLCSA: Error creating suffix array sample file!" << std::endl;
      return;
    }
    this->sa_samples2->writeTo(sa_sample_file2);
    sa_sample_file2.close();

    sa_sample_name = base_name + ".256" + SA_SAMPLES_EXTENSION;
    std::ofstream sa_sample_file3(sa_sample_name.c_str(), std::ios_base::binary);
    if(!sa_sample_file3)
    {
      std::cerr << "RLCSA: Error creating suffix array sample file!" << std::endl;
      return;
    }
    this->sa_samples3->writeTo(sa_sample_file3);
    sa_sample_file3.close();

    sa_sample_name = base_name + ".512" + SA_SAMPLES_EXTENSION;
    std::ofstream sa_sample_file4(sa_sample_name.c_str(), std::ios_base::binary);
    if(!sa_sample_file4)
    {
      std::cerr << "RLCSA: Error creating suffix array sample file!" << std::endl;
      return;
    }
    this->sa_samples4 ->writeTo(sa_sample_file4);
    sa_sample_file4.close();

    sa_sample_name = base_name + ".32" + SA_SAMPLES_EXTENSION;
    std::ofstream sa_sample_file5(sa_sample_name.c_str(), std::ios_base::binary);
    if(!sa_sample_file5)
    {
      std::cerr << "RLCSA: Error creating suffix array sample file!" << std::endl;
      return;
    }
    this->sa_samples5->writeTo(sa_sample_file5);
    sa_sample_file5.close();

    sa_sample_name = base_name + ".1024" + SA_SAMPLES_EXTENSION;
    std::ofstream sa_sample_file6(sa_sample_name.c_str(), std::ios_base::binary);
    if(!sa_sample_file6)
    {
      std::cerr << "RLCSA: Error creating suffix array sample file!" << std::endl;
      return;
    }
    this->sa_samples6 ->writeTo(sa_sample_file6);
    sa_sample_file6.close();

    sa_sample_name = base_name + ".2048" + SA_SAMPLES_EXTENSION;
    std::ofstream sa_sample_file7(sa_sample_name.c_str(), std::ios_base::binary);
    if(!sa_sample_file7)
    {
      std::cerr << "RLCSA: Error creating suffix array sample file!" << std::endl;
      return;
    }
    this->sa_samples7->writeTo(sa_sample_file7);
    sa_sample_file7.close();
  }
}

bool
RLCSA::isOk() const
{
  return this->ok;
}

//--------------------------------------------------------------------------

pair_type
RLCSA::count(const std::string& pattern) const
{
  if(pattern.length() == 0) { return pair_type(0, this->data_size - 1); }

  std::string::const_reverse_iterator iter = pattern.rbegin();

  pair_type index_range = this->index_ranges[(uchar)*iter];
  if(isEmpty(index_range)) { return EMPTY_PAIR; }
  index_range.first += this->number_of_sequences;
  index_range.second += this->number_of_sequences;

  for(++iter; iter != pattern.rend(); ++iter)
  {
    index_range = this->backwardSearchStep(index_range, (uchar)*iter);
    if(isEmpty(index_range)) { return EMPTY_PAIR; }
  }

  // Suffix array indexes are 0-based.
  index_range.first -= this->number_of_sequences;
  index_range.second -= this->number_of_sequences;

  return index_range;
}

//--------------------------------------------------------------------------

void
RLCSA::reportPositions(uchar* data, usint length, usint* positions) const
{
  if(data == 0 || length == 0 || positions == 0) { return; }

  RLEVector::Iterator** iters = this->getIterators();

  usint current = this->number_of_sequences - 1;
  positions[length] = current; // "immediately after current"
  for(sint i = (sint)(length - 1); i >= 0; i--)
  {
    usint c = (usint)data[i];
    if(this->array[c] != 0)
    {
      current = this->LF(current, c, *(iters[c]));
    }
    else
    {
      if(c < this->text_chars[0]) // No previous characters either.
      {
        current = this->number_of_sequences - 1;
      }
      else
      {
        current = this->index_ranges[c].first - 1 + this->number_of_sequences;
      }
    }
    positions[i] = current; // "immediately after current"
  }

  this->deleteIterators(iters);
}

//--------------------------------------------------------------------------

usint*
RLCSA::locate(pair_type range) const
{
    unsigned int occs;
  if(!(this->support_locate) || isEmpty(range) || range.second >= this->data_size) { return 0; }

  usint* data = new usint[length(range)];
  this->locateUnsafe(range, data, 0,&occs);

  return data;
}
usint*
RLCSA::locate(pair_type range, unsigned int max_occs, unsigned int* occs) const
{
  if(!(this->support_locate) || isEmpty(range) || range.second >= this->data_size) { return 0; }

  usint* data = new usint[length(range)];
  this->locateUnsafe(range, data, max_occs,occs);

  return data;
}

usint*
RLCSA::locate(pair_type range, usint* data) const
{
    unsigned int occs;
  if(!(this->support_locate) || isEmpty(range) || range.second >= this->data_size || data == 0) { return 0; }
  this->locateUnsafe(range, data, 0, &occs);
  return data;
}

void
RLCSA::locateUnsafe(pair_type range, usint* data, unsigned int max_occs,unsigned int* occs) const
{
  usint items = length(range);
  if(max_occs>0)
    items = std::min(max_occs,items);
  *occs=items;
  usint* offsets = new usint[items];
  bool* finished = new bool[items];  // FIXME This could be more space efficient...

  RLEVector::Iterator** iters = this->getIterators();

  for(usint i = 0, j = range.first; i < items; i++, j++)
  {
    data[i] = j + this->number_of_sequences;
    offsets[i] = 0;
    finished[i] = false;
  }

  bool found = false;
  while(!found)
  {
    found = true;
    pair_type run = EMPTY_PAIR;
    for(usint i = 0; i < items; i++)
    {
      if(finished[i])
      {
        continue; // The run might continue after this.
      }
      else if(isEmpty(run))
      {
        run = pair_type(i, i);
      }
      else if(data[i] - data[run.first] == i - run.first)
      {
        run.second = i;
      }
      else
      {
        found &= this->processRun(run, data, offsets, finished, iters);
        run = pair_type(i, i);
      }
    }
    if(!isEmpty(run)) { found &= this->processRun(run, data, offsets, finished, iters); }
  }

  this->deleteIterators(iters);
  delete[] offsets;
  delete[] finished;
}

bool
RLCSA::processRun(pair_type run, usint* data, usint* offsets, bool* finished, RLEVector::Iterator** iters) const
{
  bool found = true;
  usint run_start = 0, run_left = 0;
  pair_type next_sample = pair_type(0, 0);

  for(usint i = run.first; i <= run.second; i++)
  {
    if(finished[i])
    {
      if(run_left > 0) { run_left--; }
      continue;
    }
    if(data[i] < this->number_of_sequences) // Implicit sample here.
    {
      DeltaVector::Iterator iter(*(this->end_points));
      data[i] = iter.select(data[i]) + 1 - offsets[i];
      finished[i] = true;
      if(run_left > 0) { run_left--; }
      continue;
    }
    if(next_sample.first < data[i]) // Need another sample.
    {
      next_sample = this->sa_samples1->getFirstSampleAfter(data[i] - this->number_of_sequences);
      next_sample.first += this->number_of_sequences;
    }
    if(data[i] < next_sample.first) // No sample found for current position.
    {
      if(run_left > 0)
      {
        data[i] = data[run_start] + i - run_start;
        run_left--;
      }
      else
      {
        pair_type value = this->psi(data[i] - this->number_of_sequences, run.second - i, iters);
        data[i] = value.first;
        run_left = value.second;
        run_start = i;
      }
      offsets[i]++;
      found = false;
    }
    else  // Sampled position found.
    {
      data[i] = this->sa_samples1->getSample(next_sample.second) - offsets[i];
      finished[i] = true;
      if(run_left > 0) { run_left--; }
    }
  }
  return found;
}

usint
RLCSA::locate(usint index) const
{
  if(!(this->support_locate) || index >= this->data_size) { return this->data_size; }

  usint offset = 0;
  index += this->number_of_sequences;
  while(true)
  {
    if(index < this->number_of_sequences) // Implicit sample here
    {
      DeltaVector::Iterator iter(*(this->end_points));
      return iter.select(index) + 1 - offset;
    }
    pair_type next_sample = this->sa_samples1->getFirstSampleAfter(index - this->number_of_sequences);
    next_sample.first += this->number_of_sequences;
    if(next_sample.first == index)
    {
      return this->sa_samples1->getSample(next_sample.second) - offset;
    }
    index = this->psi(index - this->number_of_sequences);
    offset++;
  }
}

//--------------------------------------------------------------------------

uchar*
RLCSA::display(usint sequence) const
{
  if(!(this->support_display)) { return 0; }

  pair_type seq_range = this->getSequenceRange(sequence);
  if(isEmpty(seq_range)) { return 0; }

  uchar* data = new uchar[length(seq_range)];
  this->displayUnsafe(seq_range, data);

  return data;
}

uchar*
RLCSA::display(usint sequence, pair_type range) const
{
  if(!(this->support_display) || isEmpty(range)) { return 0; }

  pair_type seq_range = this->getSequenceRange(sequence);
  if(isEmpty(seq_range)) { return 0; }

  range.first += seq_range.first; range.second += seq_range.first;
  if(range.second > seq_range.second) { return 0; }

  uchar* data = new uchar[length(range)];
  this->displayUnsafe(range, data);

  return data;
}

uchar*
RLCSA::display(usint sequence, pair_type range, uchar* data) const
{
  if(!(this->support_display) || isEmpty(range) || data == 0) { return 0; }

  pair_type seq_range = this->getSequenceRange(sequence);
  if(isEmpty(seq_range)) { return 0; }

  range.first += seq_range.first; range.second += seq_range.first;
  if(range.second > seq_range.second) { return 0; }

  this->displayUnsafe(range, data);
  return data;
}

uchar*
RLCSA::display(usint position, usint len, usint context, usint& result_length) const
{
  if(!(this->support_display)) { return 0; }

  pair_type range = this->getSequenceRangeForPosition(position);
  if(isEmpty(range)) { return 0; }

  range.first = position - std::min(context, position - range.first);
  range.second = std::min(range.second, position + len + context - 1);
  result_length = length(range);
  if(isEmpty(range)) { return 0; }

  uchar* data = new uchar[length(range)];
  this->displayUnsafe(range, data);

  return data;
}

usint
RLCSA::displayPrefix(usint sequence, usint len, uchar* data) const
{
  if(!(this->support_display) || len == 0 || data == 0) { return 0; }

  pair_type seq_range = this->getSequenceRange(sequence);
  if(isEmpty(seq_range)) { return 0; }

  pair_type range(seq_range.first, std::min(seq_range.second, seq_range.first + len - 1));

  this->displayUnsafe(range, data);
  return length(range);
}

usint
RLCSA::displayFromPosition(usint index, usint max_len, uchar* data) const
{
  if(max_len == 0 || data == 0 || index >= this->data_size) { return 0; }

  for(usint i = 0; i < max_len; i++)
  {
    data[i] = this->getCharacter(index);
    index = this->psiUnsafe(index, data[i]);
    if(index < this->number_of_sequences) { return i + 1; }
    index -= this->number_of_sequences;
  }

  return max_len;
}

void
RLCSA::displayUnsafe(pair_type range, uchar* data) const
{
  usint i = range.first - range.first % this->sa_samples1->getSampleRate();

  usint pos = this->sa_samples1->inverseSA(i);

  for(; i < range.first; i++)
  {
    pos = this->psi(pos) - this->number_of_sequences;
  }
  for(; i <= range.second; i++)
  {
    data[i - range.first] = this->getCharacter(pos);
    pos = this->psi(pos) - this->number_of_sequences;
  }
}

//--------------------------------------------------------------------------

pair_type
RLCSA::getSequenceRange(usint number) const
{
  if(number >= this->number_of_sequences) { return EMPTY_PAIR; }

  pair_type result;
  DeltaVector::Iterator iter(*(this->end_points));
  if(number == 0)
  {
    result.first = 0;
    result.second = iter.select(number);
  }
  else
  {
    result.first = nextMultipleOf(this->sample_rate, iter.select(number - 1));
    result.second = iter.selectNext();
  }

  return result;
}

usint
RLCSA::getSequenceForPosition(usint value) const
{
  if(value == 0) { return 0; }
  DeltaVector::Iterator iter(*(this->end_points));
  return iter.rank(value - 1);
}

pair_type
RLCSA::getSequenceRangeForPosition(usint value) const
{
  return this->getSequenceRange(this->getSequenceForPosition(value));
}

usint*
RLCSA::getSequenceForPosition(usint* values, usint len) const
{
  if(values == 0) { return 0; }

  DeltaVector::Iterator iter(*(this->end_points));
  for(usint i = 0; i < len; i++)
  {
    if(values[i] > 0) { values[i] = iter.rank(values[i] - 1); }
  }

  return values;
}

pair_type
RLCSA::getRelativePosition(usint value) const
{
  DeltaVector::Iterator iter(*(this->end_points));
  pair_type result(0, value);

  if(value > 0) { result.first = iter.rank(value - 1); }
  if(result.first > 0)
  {
    result.second -= nextMultipleOf(this->sample_rate, iter.select(result.first - 1));
  }

  return result;
}

//--------------------------------------------------------------------------

uchar*
RLCSA::readBWT() const
{
  return this->readBWT(pair_type(0, this->data_size + this->number_of_sequences - 1));
}

uchar*
RLCSA::readBWT(pair_type range) const
{
  if(isEmpty(range) || range.second >= this->data_size + this->number_of_sequences) { return 0; }

  usint n = length(range);

  uchar* bwt = new uchar[n];
  memset(bwt, 0, n);

  for(usint c = 0; c < CHARS; c++)
  {
    if(this->array[c] != 0)
    {
      RLEVector::Iterator iter(*(this->array[c]));
      usint pos = iter.valueAfter(range.first).first;
      while(pos <= range.second)
      {
        bwt[pos - range.first] = c;
        pos = iter.selectNext();
      }
    }
  }

  return bwt;
}

usint
RLCSA::countRuns() const
{
  usint runs = 0;
  for(usint c = 0; c < CHARS; c++)
  {
    if(this->array[c] != 0)
    {
      RLEVector::Iterator iter(*(this->array[c]));
      runs += iter.countRuns();
    }
  }

  return runs;
}

//--------------------------------------------------------------------------

RLEVector::Iterator**
RLCSA::getIterators() const
{
  RLEVector::Iterator** iters = new RLEVector::Iterator*[CHARS];
  for(usint i = 0; i < CHARS; i++)
  {
    if(this->array[i] == 0) { iters[i] = 0; }
    else                    { iters[i] = new RLEVector::Iterator(*(this->array[i])); }
  }
  return iters;
}

void
RLCSA::deleteIterators(RLEVector::Iterator** iters) const
{
  if(iters == 0) { return; }
  for(usint i = 0; i < CHARS; i++) { delete iters[i]; }
  delete[] iters;
}

//--------------------------------------------------------------------------

usint
RLCSA::reportSize(bool print) const
{
  usint bytes = 0, temp = 0, bwt = 0;

  for(usint c = 0; c < CHARS; c++)
  {
    if(this->array[c])
    {
      bytes += this->array[c]->reportSize();
      bwt += this->array[c]->getCompressedSize();
    }
  }
  bytes += sizeof(*this) + this->end_points->reportSize();
  if(print)
  {
    std::cout << "RLCSA:           " << (bytes / (double)MEGABYTE) << " MB" << std::endl;
    std::cout << "  BWT only:      " << (bwt / (double)MEGABYTE) << " MB" << std::endl;
  }

  if(this->support_locate || this->support_display)
  {
    temp = this->sa_samples1->reportSize();
    if(print) { std::cout << "SA samples:      " << (temp / (double)MEGABYTE) << " MB" << std::endl; }
    bytes += temp;
  }

  if(print)
  {
    std::cout << "Total size:      " << (bytes / (double)MEGABYTE) << " MB" << std::endl;
    std::cout << std::endl;
  }

  return bytes;
}

void
RLCSA::printInfo() const
{
  double megabytes = this->data_size / (double)MEGABYTE;

  std::cout << "Sequences:       " << this->number_of_sequences << std::endl;
  std::cout << "Original size:   " << megabytes << " MB" << std::endl;
  std::cout << "Block size:      " << (this->getBlockSize() * sizeof(usint)) << " bytes" << std::endl;
  if(this->support_locate || this->support_display)
  {
    std::cout << "Sample rate:     " << this->sample_rate << std::endl;
  }
  std::cout << std::endl;
}

//--------------------------------------------------------------------------

RLEVector*
RLCSA::buildPLCP(usint block_size) const
{
  if(block_size < 2 * sizeof(usint) || block_size % sizeof(usint) != 0)
  {
    std::cerr << "PLCP: Block size must be a multiple of " << sizeof(usint) << " bytes!" << std::endl;
    return 0;
  }

  RLEVector::Iterator** iters = this->getIterators();

  RLEEncoder plcp(block_size);
  std::list<pair_type> matches;
  pair_type prev_range = EMPTY_PAIR;
  for(usint j = 0; j < this->number_of_sequences; j++)
  {
    // Encode the padding as a single run.
    pair_type seq_range = this->getSequenceRange(j);
    if(j > 0 && prev_range.second + 1 < seq_range.first)
    {
      this->encodePLCPRun(plcp, prev_range.second + 1, seq_range.first, seq_range.first - prev_range.second - 2);
    }
    prev_range = seq_range;

    usint maximal = seq_range.first;
    usint x = this->sa_samples1->inverseSA(seq_range.first), next_x;

    // Invariant: x == inverseSA(i)
    for(usint i = seq_range.first; i <= seq_range.second; i++, x = next_x)
    {
      usint c = this->getCharacter(x);

      // T[i,n] is lexicographically the first suffix beginning with c.
      if(x == this->index_ranges[c].first)
      {
        if(!matches.empty())
        {
          maximal = (*(matches.begin())).first;
          matches.clear();
        }
        this->encodePLCPRun(plcp, maximal, i + 1, i - maximal);
        maximal = i + 1;
        next_x = this->psiUnsafe(x, c, *(iters[c])) - this->number_of_sequences;
        continue;
      }

      // Process the previous left matches we are still following.
      usint low = this->index_ranges[c].first + this->number_of_sequences;
      usint start, stop; start = stop = maximal;
      for(std::list<pair_type>::iterator iter = matches.begin(); iter != matches.end();)
      {
        pair_type match = *iter;
        if(match.second < low)  // These no longer match the current character.
        {
          if(match.first < start) { start = match.first; }  // Form a single run from then.
          iter = matches.erase(iter);
        }
        else
        {
          if(match.first < stop) { stop = match.first; }  // End of the run to be encoded.
          (*iter).second = this->psiUnsafe(match.second - this->number_of_sequences, c, *(iters[c]));
          ++iter;
        }
      }
      if(start < stop) { this->encodePLCPRun(plcp, start, stop, i - start); }

      // If PLCP[i] is minimal, we add the left match to the list.
      // We add pairs of type (j, inverseSA(j) + n_of_s) as inverseSA is < 0 for end markers.
      usint next_y = this->psiUnsafe(x - 1, c, *(iters[c]));
      next_x = this->psiUnsafeNext(c, *(iters[c])) - this->number_of_sequences;
      if(next_y != next_x + this->number_of_sequences - 1)
      {
        matches.push_back(pair_type(maximal, next_y));
        maximal = i + 1;
      }
    }

    if(!matches.empty())
    {
      maximal = (*(matches.begin())).first;
      matches.clear();
    }
    if(maximal <= seq_range.second)
    {
      this->encodePLCPRun(plcp, maximal, seq_range.second + 1, seq_range.second + 1 - maximal);
    }
  }

  this->deleteIterators(iters);

  return new RLEVector(plcp, 2 * (this->end_points->getSize() + 1));
}

usint
RLCSA::sampleLCP(usint sample_rate, pair_type*& sampled_values, bool report) const
{
  if(sample_rate == 0)
  {
    sample_rate = this->data_size + 1;
  }

  RLEVector::Iterator** iters = this->getIterators();

  usint runs = this->countRuns();
  usint samples = 0, minimal_sum = 0, nonstrict_minimal_sum = 0, minimal_samples = 0;
  usint max_samples = runs + (this->data_size - runs) / sample_rate;
  sampled_values = new pair_type[max_samples];

  std::list<Triple> matches;
  for(usint j = 0; j < this->number_of_sequences; j++)
  {
    pair_type seq_range = this->getSequenceRange(j);
    usint first_sample = samples; // First minimal sample of the current sequence.
    usint minimal = seq_range.second + 1; // Last minimal position.
    usint i, x = this->sa_samples1->inverseSA(seq_range.first), next_x, prev_x = 0;

    // Invariant: x == inverseSA(i)
    for(i = seq_range.first; i <= seq_range.second; i++, x = next_x)
    {
      usint c = this->getCharacter(x);

      // T[i,n] is lexicographically the first suffix beginning with c.
      if(x == this->index_ranges[c].first)
      {
        if(!matches.empty())
        {
          for(std::list<Triple>::iterator iter = matches.begin(); iter != matches.end(); ++iter)
          {
            nonstrict_minimal_sum += i - (*iter).first;
          }
          matches.clear();
        }

        // Minimal sample: SA[x] = i, LCP[x] = 0
        sampled_values[samples] = pair_type(x, 0); minimal = i;
        samples++; minimal_samples++;
        next_x = this->psiUnsafe(x, c, *(iters[c])) - this->number_of_sequences; prev_x = x;
        continue;
      }

      // Process the previous left matches we are still following.
      usint low = this->index_ranges[c].first + this->number_of_sequences;
      pair_type potential_sample(this->data_size, this->data_size);
      for(std::list<Triple>::iterator iter = matches.begin(); iter != matches.end();)
      {
        Triple match = *iter;
        if(match.second < low)  // These no longer match the current character.
        {
          if(potential_sample.first != this->data_size) { nonstrict_minimal_sum += potential_sample.second; }

          // Potential minimal sample: SA[match.third] = match.first, LCP[match.third] = i - match.first
          potential_sample = pair_type(match.third, i - match.first);
          iter = matches.erase(iter);
        }
        else
        {
          (*iter).second = this->psiUnsafe(match.second - this->number_of_sequences, c, *(iters[c]));
          ++iter;
        }
      }
      if(potential_sample.first != this->data_size)
      {
        // Last potential sample is minimal.
        sampled_values[samples] = potential_sample;
        samples++; minimal_sum += potential_sample.second; minimal_samples++;
      }

      // If PLCP[i] is minimal, we add the left match to the list.
      // We add triples of type (j, inverseSA(j) + n_of_s, x) as inverseSA is < 0 for end markers.
      // x is the original minimal position.
      usint next_y = this->psiUnsafe(x - 1, c, *(iters[c]));
      next_x = this->psiUnsafeNext(c, *(iters[c]));
      if(next_y != next_x - 1 || next_x < this->number_of_sequences)
      {
        matches.push_back(Triple(i, next_y, x)); minimal = i;
      }
      next_x -= this->number_of_sequences; prev_x = x;
    }

    // Are we still following something?
    if(!matches.empty())
    {
      pair_type potential_sample(this->data_size, this->data_size);
      for(std::list<Triple>::iterator iter = matches.begin(); iter != matches.end(); ++iter)
      {
        Triple match = *iter;
        if(potential_sample.first != this->data_size) { nonstrict_minimal_sum += potential_sample.second; }

        // Potential minimal sample: SA[match.third] = match.first, LCP[match.third] = i - match.first
        potential_sample = pair_type(match.third, i - match.first);
      }
      matches.clear();
      if(potential_sample.first != this->data_size)
      {
        // Last potential sample is minimal.
        sampled_values[samples] = potential_sample;
        samples++; minimal_sum += potential_sample.second; minimal_samples++;
      }
    }

    // Add the non-minimal samples.
    if(sample_rate <= this->data_size)
    {
      usint last_sample = samples - 1;
      i = seq_range.first; x = this->sa_samples1->inverseSA(seq_range.first);
      for(usint current_sample = first_sample; current_sample <= last_sample; current_sample++)
      {
        // Find the next minimal sample and add nonminimal samples if needed.
        pair_type first_nonminimal(i + sample_rate - 1, samples);
        pair_type next_nonminimal = first_nonminimal;
        while(x != sampled_values[current_sample].first)
        {
          if(i == next_nonminimal.first)
          {
            sampled_values[samples] = pair_type(x, 0); samples++;
            next_nonminimal.first += sample_rate; next_nonminimal.second++;
          }
          i++; x = this->psi(x, iters) - this->number_of_sequences;
        }

        // Reduce the nonminimal samples to the current minimal sample.
        for(next_nonminimal = first_nonminimal; next_nonminimal.second < samples; next_nonminimal.first += sample_rate, next_nonminimal.second++)
        {
          sampled_values[next_nonminimal.second].second = sampled_values[current_sample].second + i - next_nonminimal.first;
        }
        i++; x = this->psi(x, iters) - this->number_of_sequences;
      }
    }
  }

  std::sort(sampled_values, sampled_values + samples);

  if(report)
  {
    std::cout << "Samples: " << samples << " (total) / " << minimal_samples << " (minimal)" << std::endl;
    std::cout << "Upper bounds: " << max_samples << " (total) / " << runs << " (minimal)" << std::endl;
    std::cout << "Sum of minimal samples: " << (minimal_sum + nonstrict_minimal_sum) << " (total) / " << minimal_sum << " (strict)" << std::endl;
    std::cout << std::endl;
  }

  this->deleteIterators(iters);

  return samples;
}

usint
RLCSA::lcp(usint index, const LCPSamples& lcp_samples) const
{
  if(index >= this->data_size) { return this->data_size; }

  usint offset = 0;
  index += this->number_of_sequences;
  while(true)
  {
    pair_type next_sample = lcp_samples.getFirstSampleAfter(index - this->number_of_sequences);
    next_sample.first += this->number_of_sequences;
    if(next_sample.first == index)
    {
      return lcp_samples.getSample(next_sample.second) + offset;
    }
    index = this->psi(index - this->number_of_sequences);
    offset++;
  }
}

usint
RLCSA::lcp(usint index, const LCPSamples& lcp_samples, const RLEVector& plcp) const
{
  if(index >= this->data_size) { return this->data_size; }

  usint offset = 0;
  while(true)
  {
    usint c = this->getCharacter(index);
    RLEVector::Iterator iter(*(this->array[c]));

    if(index == this->index_ranges[c].first)
    {
      return lcp_samples.getSampleAtPosition(index) + offset;
    }
    usint next_y = this->psiUnsafe(index - 1, c, iter);
    usint next_x = this->psiUnsafeNext(c, iter);
    if(next_y != next_x - 1 || next_x < this->number_of_sequences)
    {
      pair_type sample = lcp_samples.getFirstSampleAfter(index);
      if(sample.first == index)
      {
        return lcp_samples.getSample(sample.second) + offset;
      }
    }

    pair_type next_sample = this->sa_samples1->getFirstSampleAfter(index);
    if(next_sample.first == index)
    {
      index = this->sa_samples1->getSample(next_sample.second) - offset;
      RLEVector::Iterator plcp_iter(plcp);
      return plcp_iter.select(index) - 2 * index;
    }
    index = next_x - this->number_of_sequences;
    offset++;
  }
}

usint
RLCSA::lcpDirect(usint index) const
{
  if(index >= this->data_size || index == 0)
  {
    return 0;
  }

  usint match = index - 1;
  usint value = 0;

  usint c = this->getCharacter(index);
  usint low = this->index_ranges[c].first;
  while(match >= low)
  {
    RLEVector::Iterator iter(*(this->array[c]));
    match = this->psiUnsafe(match, c, iter);
    index = this->psiUnsafe(index, c, iter);
    value++;

    if(match < this->number_of_sequences || index < this->number_of_sequences)
    {
      break;
    }
    match -= this->number_of_sequences;
    index -= this->number_of_sequences;

    c = this->getCharacter(index);
    low = this->index_ranges[c].first;
  }

  return value;
}

//--------------------------------------------------------------------------

void
RLCSA::mergeEndPoints(RLCSA& index, RLCSA& increment)
{
  DeltaEncoder* endings = new DeltaEncoder(RLCSA::ENDPOINT_BLOCK_SIZE);

  DeltaVector::Iterator index_iter(*(index.end_points));
  DeltaVector::Iterator increment_iter(*(increment.end_points));

  endings->setBit(index_iter.select(0));
  for(usint i = 1; i < index.number_of_sequences; i++)
  {
    endings->setBit(index_iter.selectNext());
  }
  usint sum = index.end_points->getSize();
  delete index.end_points; index.end_points = 0;

  endings->setBit(sum + increment_iter.select(0));
  for(usint i = 1; i < increment.number_of_sequences; i++)
  {
    endings->setBit(sum + increment_iter.selectNext());
  }
  sum += increment.end_points->getSize();
  delete increment.end_points; increment.end_points = 0;

  this->end_points = new DeltaVector(*endings, sum);
  delete endings;
}


void
RLCSA::mergeSamples(RLCSA& index, RLCSA& increment, usint* positions)
{
  if(this->support_locate || this->support_display)
  {
    positions += increment.number_of_sequences;
    this->sa_samples1 = new SASamples(*(index.sa_samples1), *(increment.sa_samples1), positions, increment.data_size, this->number_of_sequences);
  }
}

//--------------------------------------------------------------------------

void
RLCSA::buildCharIndexes(usint* distribution)
{
  this->data_size = buildRanges(distribution, this->index_ranges);

  usint i = 0, c = 0;
  for(; c < CHARS; c++)
  {
    if(!isEmpty(this->index_ranges[c]))
    {
      this->text_chars[i] = c;
      i++;
    }
  }
  this->chars = i;

  this->index_rate = std::max((this->data_size + CHARS - 1) / CHARS, (usint)1);
  usint current = 0;

  for(c = 0, i = 0; c < this->chars; c++)
  {
    pair_type range = this->index_ranges[this->text_chars[c]];
    while(current <= range.second)
    {
      this->index_pointers[i] = c;
      current += this->index_rate;
      i++;
    }
  }
}

usint
buildRanges(usint* distribution, pair_type* index_ranges)
{
  if(distribution == 0 || index_ranges == 0) { return 0; }

  usint sum = 0;
  for(usint c = 0; c < CHARS; c++)
  {
    if(distribution[c] == 0)
    {
      if(sum == 0) { index_ranges[c] = EMPTY_PAIR; }
      else         { index_ranges[c] = pair_type(sum, sum - 1); }
    }
    else
    {
      index_ranges[c].first = sum;
      sum += distribution[c];
      index_ranges[c].second = sum - 1;
    }
  }

  return sum;
}


} // namespace CSA
