#ifndef LCPSAMPLES_H
#define LCPSAMPLES_H

#include <fstream>

#include "bits/array.h"
#include "bits/vectors.h"
#include "misc/utils.h"


namespace CSA
{


class LCPSamples
{
  public:
    const static usint INDEX_BLOCK_SIZE = 16;
    const static usint VALUE_BLOCK_SIZE = 32;

    LCPSamples(std::ifstream& sample_file);

    // Input pairs are of form (i, LCP[i]).
    LCPSamples(pair_type* input, usint _size, usint _items, bool report = false, bool free_input = false);

    LCPSamples(DeltaEncoder& index_encoder, ArrayEncoder& value_encoder, usint _size);

    ~LCPSamples();

    void writeTo(std::ofstream& sample_file) const;

    // Returns the value of the ith sample in suffix array order.
    // If the index is invalid, returns this->size.
    inline usint getSample(usint i) const
    {
      Array::Iterator iter(*(this->values));
      usint tmp = iter.getItem(i);
      if(tmp == 0) { return this->size; }
      return tmp - 1;
    }

    // Returns (ind, sample number) where ind >= index or (size, ???).
    inline pair_type getFirstSampleAfter(usint index) const
    {
      DeltaVector::Iterator iter(*(this->indexes));
      return iter.valueAfter(index);
    }

    // Behavior is undefined if there is no sample at index.
    inline usint getSampleAtPosition(usint index) const
    {
      DeltaVector::Iterator index_iter(*(this->indexes));
      Array::Iterator value_iter(*(this->values));
      return value_iter.getItem(index_iter.rank(index) - 1) - 1;
    }

    inline usint getNumberOfSamples() const { return this->items; }

    usint reportSize() const;

  private:
    usint size, items;

    DeltaVector* indexes;
    Array*       values;

    // These are not allowed.
    LCPSamples();
    LCPSamples(const LCPSamples&);
    LCPSamples& operator = (const LCPSamples&);
};


} // namespace CSA


#endif // LCPSAMPLES_H
