#ifndef SASAMPLES_H
#define SASAMPLES_H

#include <fstream>

#include "misc/definitions.h"
#include "bits/bitbuffer.h"
#include "bits/deltavector.h"


namespace CSA
{


class SASamples
{
  public:
    const static usint INDEX_BLOCK_SIZE = 16;

    SASamples(std::ifstream& sample_file, usint sample_rate);

    // These assume < 2 GB data. Use the second one when there are multiple sequences.
    SASamples(uint* array, usint data_size, usint sample_rate);
    SASamples(uint* inverse, DeltaVector* end_points, usint data_size, usint sample_rate);

    ~SASamples();

    // Destroys contents of index and increment.
    // We assume index and increment have same sample rate.
    // positions must not containt the positions of end of sequence markers.
    // number_of_sequences is subtracted from each position before the value is used.
    SASamples(SASamples& index, SASamples& increment, usint* positions, usint number_of_positions, usint number_of_sequences);

    void writeTo(std::ofstream& sample_file) const;

    // Returns i such that SA[i] = value.
    // If SA[i] is not sampled, returns the next sampled value. (Don't try!)
    // Value is actual 0-based suffix array value.
    // Returns size if value is too large.
    inline usint inverseSA(usint value) const
    {
      if(value >= this->size) { return this->size; }
      DeltaVector::Iterator iter(*(this->indexes));
      return iter.select(this->inverse_samples->readItemConst(value / this->rate));
    }

    // Returns the value of ith sample in suffix array order.
    inline usint getSample(usint i) const
    {
      return std::min(this->samples->readItemConst(i) * this->rate, this->size - 1);
    }

    // Returns (ind, sample number) where ind >= index or (size, ???).
    inline pair_type getFirstSampleAfter(usint index) const
    {
      DeltaVector::Iterator iter(*(this->indexes));
      return iter.valueAfter(index);
    }

    inline usint getSampleRate() const { return this->rate; }
    inline usint getNumberOfSamples() const { return this->items; }

    usint reportSize() const;

  private:
    usint integer_bits;
    usint rate, size, items;

    DeltaVector* indexes;

    ReadBuffer* samples;
    ReadBuffer* inverse_samples;

    void buildInverseSamples();

    // Note: contents of original samples are deleted.
    void mergeSamples(SASamples& index, SASamples& increment, usint* positions, usint n, usint skip);

    // These are not allowed.
    SASamples();
    SASamples(const SASamples&);
    SASamples& operator = (const SASamples&);
};


} // namespace CSA


#endif // SASAMPLES_H
