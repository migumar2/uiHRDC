#include <algorithm>
#include <fstream>
#include <iostream>
#include <vector>

#include "sasamples.h"
#include "misc/utils.h"

#ifdef MULTITHREAD_SUPPORT
#include <mcstl.h>
#endif


namespace CSA
{


SASamples::SASamples(std::ifstream& sample_file, usint sample_rate) :
  rate(sample_rate)
{
  this->indexes = new DeltaVector(sample_file);

  this->size = indexes->getSize();
  this->items = indexes->getNumberOfItems();
  this->integer_bits = length(this->items - 1);

  this->samples = new ReadBuffer(sample_file, this->items, this->integer_bits);
  this->buildInverseSamples();
}

SASamples::SASamples(uint* array, usint data_size, usint sample_rate) :
  rate(sample_rate),
  size(data_size)
{
  this->items = (this->size - 1) / this->rate + 1;
  this->integer_bits = length(this->items - 1);

  WriteBuffer sample_buffer(this->items, this->integer_bits);
  DeltaEncoder encoder(INDEX_BLOCK_SIZE);
  for(usint i = 0; i < this->size; i++)
  {
    if(array[i] % sample_rate == 0)
    {
      encoder.setBit(i);
      sample_buffer.writeItem(array[i] / sample_rate);
    }
  }

  this->indexes = new DeltaVector(encoder, this->size);
  this->samples = sample_buffer.getReadBuffer();
  this->buildInverseSamples();
}

SASamples::SASamples(uint* inverse, DeltaVector* end_points, usint data_size, usint sample_rate) :
  rate(sample_rate),
  items(0)
{
  usint sequences = end_points->getNumberOfItems();

  DeltaVector::Iterator iter(*(end_points));

  // Determine the samples, insert them into a vector, and sort them.
  usint start = 0, end = iter.select(0);  // Closed range in padded collection.
  usint seq_start = 0, seq_end = end;            // Closed range in inverse SA.
  std::vector<pair_type>* vec = new std::vector<pair_type>();
  for(usint i = 0; i < sequences; i++)
  {
    for(usint j = seq_start; j <= seq_end; j += this->rate)
    {
      vec->push_back(pair_type(inverse[j] - sequences - 1, this->items));
      this->items++;
    }
    start = nextMultipleOf(this->rate, end);
    end = iter.selectNext();
    seq_start = seq_end + 2;
    seq_end = seq_start + end - start;
  }
  #ifdef MULTITHREAD_SUPPORT
  std::sort(vec->begin(), vec->end(), mcstl::sequential_tag());
  #else
  std::sort(vec->begin(), vec->end());
  #endif

  // Compress the samples.
  this->integer_bits = length(this->items - 1);
  this->size = end + 1;
  WriteBuffer sample_buffer(this->items, this->integer_bits);
  DeltaEncoder encoder(INDEX_BLOCK_SIZE);
  for(usint i = 0; i < this->items; i++)
  {
    pair_type sample = (*vec)[i];
    encoder.setBit(sample.first);
    sample_buffer.writeItem(sample.second);
  }
  delete vec;

  this->indexes = new DeltaVector(encoder, this->size);
  this->samples = sample_buffer.getReadBuffer();
  this->buildInverseSamples();
}

SASamples::SASamples(SASamples& index, SASamples& increment, usint* positions, usint number_of_positions, usint number_of_sequences) :
  rate(index.rate),
  size(index.size + increment.size),
  items(index.items + increment.items)
{
  this->mergeSamples(index, increment, positions, number_of_positions, number_of_sequences);

  index.indexes = 0;
  index.samples = 0;
  index.inverse_samples = 0;
  increment.indexes = 0;
  increment.samples = 0;
  increment.inverse_samples = 0;

  this->buildInverseSamples();
}

SASamples::~SASamples()
{
  delete this->indexes;
  delete this->samples;
  delete this->inverse_samples;
}

//--------------------------------------------------------------------------

void
SASamples::writeTo(std::ofstream& sample_file) const
{
  this->indexes->writeTo(sample_file);
  this->samples->writeBuffer(sample_file);
}

usint
SASamples::reportSize() const
{
  usint bytes = sizeof(*this);
  bytes += this->indexes->reportSize();
  bytes += this->samples->reportSize();
  bytes += this->inverse_samples->reportSize();
  return bytes;
}

//--------------------------------------------------------------------------

void
SASamples::buildInverseSamples()
{
  WriteBuffer inverse_buffer(this->items, this->integer_bits);
  this->samples->goToItem(0);
  for(usint i = 0; i < this->items; i++)
  {
    inverse_buffer.goToItem(this->samples->readItem());
    inverse_buffer.writeItem(i);
  }

  this->inverse_samples = inverse_buffer.getReadBuffer();
}


//--------------------------------------------------------------------------

void
SASamples::mergeSamples(SASamples& index, SASamples& increment, usint* positions, usint n, usint skip)
{
  DeltaVector::Iterator first(*(index.indexes));
  DeltaVector::Iterator second(*(increment.indexes));
  ReadBuffer* first_samples = index.samples;
  ReadBuffer* second_samples = increment.samples;

  usint first_bit = first.select(0);
  bool first_finished = false;
  usint second_bit = second.select(0);
  usint sum = index.items;
  first_samples->goToItem(0);
  second_samples->goToItem(0);

  DeltaEncoder encoder(INDEX_BLOCK_SIZE);
  this->integer_bits = length(this->items - 1);
  WriteBuffer sample_buffer(this->items, this->integer_bits);
  for(usint i = 0; i < n; i++, first_bit++)
  {
    while(!first_finished && first_bit < positions[i] - skip)
    {
      encoder.setBit(first_bit);
      sample_buffer.writeItem(first_samples->readItem());
      if(first.hasNext())
      {
        first_bit = first.selectNext() + i;
      }
      else
      {
        first_finished = true;
      }
    }

    if(i == second_bit) // positions[i] is one
    {
      encoder.setBit(positions[i] - skip);
      sample_buffer.writeItem(second_samples->readItem() + sum);
      second_bit = second.selectNext();
    }
  }

  while(!first_finished)
  {
    encoder.setBit(first_bit);
    sample_buffer.writeItem(first_samples->readItem());
    if(!first.hasNext()) { break; }
    first_bit = first.selectNext() + n;
  }

  delete index.indexes;
  delete index.samples;
  delete index.inverse_samples;
  delete increment.indexes;
  delete increment.samples;
  delete increment.inverse_samples;

  this->indexes = new DeltaVector(encoder, size);
  this->samples = sample_buffer.getReadBuffer();
}


} // namespace CSA
