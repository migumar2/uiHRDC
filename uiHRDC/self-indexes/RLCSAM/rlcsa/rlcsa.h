#ifndef RLCSA_H
#define RLCSA_H

#include <fstream>
#include <vector>

#include "bits/vectors.h"
#include "sasamples.h"
#include "lcpsamples.h"
#include "misc/parameters.h"


namespace CSA
{


const std::string SA_EXTENSION = ".sa";
const std::string PSI_EXTENSION = ".psi";
const std::string ARRAY_EXTENSION = ".rlcsa.array";
const std::string SA_SAMPLES_EXTENSION = ".rlcsa.sa_samples";
const std::string PARAMETERS_EXTENSION = ".rlcsa.parameters";
const std::string LCP_SAMPLES_EXTENSION = ".lcp_samples";
const std::string PLCP_EXTENSION = ".plcp";


const parameter_type RLCSA_BLOCK_SIZE  = parameter_type("RLCSA_BLOCK_SIZE", 32);
const parameter_type SAMPLE_RATE       = parameter_type("SAMPLE_RATE", 512);
const parameter_type SUPPORT_LOCATE    = parameter_type("SUPPORT_LOCATE", 1);
const parameter_type SUPPORT_DISPLAY   = parameter_type("SUPPORT_DISPLAY", 1);


class RLCSA
{
  public:

//--------------------------------------------------------------------------
//  CONSTRUCTION
//--------------------------------------------------------------------------

    const static usint ENDPOINT_BLOCK_SIZE = 16;

    RLCSA(const std::string& base_name, bool print = false, char* ssample="512");

    /*
      If multiple_sequences is true, each 0 is treated as a end of sequence marker.
      There must be nonzero characters between the 0s. data[bytes - 1] must also be 0.
      FIXME Crashes if bytes >= 2 GB.
    */ 
    RLCSA(uchar* data, usint bytes, usint block_size, usint sa_sample_rate, bool multiple_sequences, bool delete_data);

    // Destroys contents of index and increment.
    // threads has no effect unless MULTITHREAD_SUPPORT is defined
    RLCSA(RLCSA& index, RLCSA& increment, usint* positions, usint block_size, usint threads = 1);
    ~RLCSA();

    void writeTo(const std::string& base_name) const;

    bool isOk() const;

//--------------------------------------------------------------------------
//  QUERIES
//--------------------------------------------------------------------------

    // Returns the closed interval of suffix array containing the matches.
    pair_type count(const std::string& pattern) const;

    // Used when merging CSAs.
    void reportPositions(uchar* data, usint length, usint* positions) const;

    // Returns SA[range]. User must free the buffer. Latter version uses buffer provided by the user.
    usint* locate(pair_type range) const;
    usint* locate(pair_type range, unsigned int max_occs,unsigned int* occs) const;
    usint* locate(pair_type range, usint* data) const;

    // Returns SA[index].
    usint locate(usint index) const;

    // Returns T^{sequence}[range]. User must free the buffer.
    // Third version uses buffer provided by the user.
    uchar* display(usint sequence) const;
    uchar* display(usint sequence, pair_type range) const;
    uchar* display(usint sequence, pair_type range, uchar* data) const;

    // Displays the intersection of T[position - context, position + len + context - 1]
    // and T^{getSequenceForPosition(position)}.
    // This is intended for displaying an occurrence of a pattern of length 'len'
    // with 'context' extra characters on both sides.
    // The actual length of the returned string is written into result_length.
    uchar* display(usint position, usint len, usint context, usint& result_length) const;

    // Returns the actual length of the prefix. User must provide the buffer.
    usint displayPrefix(usint sequence, usint len, uchar* data) const;

    // Returns at most max_len characters starting from T[SA[index]].
    // User must provide the buffer. Returns the number of characters in buffer.
    usint displayFromPosition(usint index, usint max_len, uchar* data) const;

    // Get the range of SA values for the sequence identified by
    // a sequence number or a SA value.
    pair_type getSequenceRange(usint number) const;
    pair_type getSequenceRangeForPosition(usint value) const;

    // Get the sequence number for given SA value(s).
    // The returned array is the same as the parameter.
    usint getSequenceForPosition(usint value) const;
    usint* getSequenceForPosition(usint* value, usint length) const;

    // Changes SA value to (sequence, offset).
    pair_type getRelativePosition(usint value) const;

    // Returns the BWT of the collection including end of sequence markers.
    uchar* readBWT() const;
    uchar* readBWT(pair_type range) const;

    // Returns the number of equal letter runs in the BWT. Runs consisting of end markers are ignored.
    usint countRuns() const;

//--------------------------------------------------------------------------
//  BASIC OPERATIONS
//--------------------------------------------------------------------------

    // The return values of these functions include the implicit end markers.
    // To get SA indexes, subtract getNumberOfSequences() from the value.

    inline usint psi(usint index) const
    {
      if(index >= this->data_size)
      {
        return this->data_size + this->number_of_sequences;
      }

      usint c = this->getCharacter(index);
      return this->psiUnsafe(index, c);
    }

    // This version returns a run.
    inline pair_type psi(usint index, usint max_length) const
    {
      if(index >= this->data_size)
      {
        return pair_type(this->data_size + this->number_of_sequences, 0);
      }

      usint c = this->getCharacter(index);
      RLEVector::Iterator iter(*(this->array[c]));
      return iter.selectRun(index - this->index_ranges[c].first, max_length);
    }

    inline usint LF(usint index, usint c) const
    {
      if(c >= CHARS)
      {
        return this->data_size + this->number_of_sequences;
      }
      if(this->array[c] == 0)
      {
        if(c < this->text_chars[0]) { return this->number_of_sequences - 1; }
        return this->index_ranges[c].first + this->number_of_sequences - 1;
      }
      index += this->number_of_sequences;

      RLEVector::Iterator iter(*(this->array[c]));
      return this->LF(index, c, iter);
    }

//--------------------------------------------------------------------------
//  REPORTING
//--------------------------------------------------------------------------

    inline bool supportsLocate() const { return this->support_locate; }
    inline bool supportsDisplay() const { return this->support_display; }
    inline usint getSize() const { return this->data_size; }
    inline usint getNumberOfSequences() const { return this->number_of_sequences; }
    inline usint getBlockSize() const { return this->array[this->text_chars[0]]->getBlockSize(); }

    // Returns the size of the data structure.
    usint reportSize(bool print = false) const;

    void printInfo() const;

//--------------------------------------------------------------------------
//  LCP EXPERIMENTS
//--------------------------------------------------------------------------

    // Optimized version:
    //   - Interleaves main loop with computing irreducible values.
    //   - Encodes maximal runs from a true local maximum to a true local minimum.
    RLEVector* buildPLCP(usint block_size) const;

    // Returns the number of samples. sampled_values will be a pointer to the samples.
    usint sampleLCP(usint sample_rate, pair_type*& sampled_values, bool report = false) const;

    usint lcp(usint index, const LCPSamples& lcp_samples) const;
    usint lcp(usint index, const LCPSamples& lcp_samples, const RLEVector& plcp) const;

    // Calculate LCP[index] directly.
    usint lcpDirect(usint index) const;

    // Writes PLCP[start] to PLCP[stop - 1].
    inline void encodePLCPRun(RLEEncoder& plcp, usint start, usint stop, usint first_val) const
    {
      plcp.setRun(2 * start + first_val, stop - start);
//      std::cerr << "(" << start << ", " << stop << ", " << first_val << ")" << std::endl;
    }

//--------------------------------------------------------------------------
//  INTERNAL VARIABLES
//--------------------------------------------------------------------------

  private:
    bool ok;

    RLEVector* array[CHARS];
    SASamples* sa_samples1;
    SASamples* sa_samples2;
    SASamples* sa_samples3;
    SASamples* sa_samples4;
    SASamples* sa_samples5;
    SASamples* sa_samples6;
    SASamples* sa_samples7;

    pair_type index_ranges[CHARS];
    usint data_size;

    usint text_chars[CHARS];  // which characters are present in the text
    usint chars;

    usint index_pointers[CHARS]; // which of the above is at i * index_rate
    usint index_rate;

    bool support_locate, support_display;
    usint sample_rate;

    usint number_of_sequences;
    DeltaVector* end_points;

//--------------------------------------------------------------------------
//  INTERNAL VERSIONS OF QUERIES
//--------------------------------------------------------------------------

    void locateUnsafe(pair_type range, usint* data, unsigned int max_occs,unsigned int* occs) const;
    bool processRun(pair_type run, usint* data, usint* offsets, bool* finished, RLEVector::Iterator** iters) const;
    void displayUnsafe(pair_type range, uchar* data) const;

//--------------------------------------------------------------------------
//  INTERNAL VERSIONS OF BASIC OPERATIONS
//--------------------------------------------------------------------------

    inline usint psi(usint index, RLEVector::Iterator** iters) const
    {
      usint c = this->getCharacter(index);
      return iters[c]->select(index - this->index_ranges[c].first);
    }

    inline pair_type psi(usint index, usint max_length, RLEVector::Iterator** iters) const
    {
      usint c = this->getCharacter(index);
      return iters[c]->selectRun(index - this->index_ranges[c].first, max_length);
    }

    // Returns psi(index), assuming the suffix of rank index begins with c.
    inline usint psiUnsafe(usint index, usint c) const
    {
      RLEVector::Iterator iter(*(this->array[c]));
      return this->psiUnsafe(index, c, iter);
    }

    // As above, but with a given iterator.
    inline usint psiUnsafe(usint index, usint c, RLEVector::Iterator& iter) const
    {
      return iter.select(index - this->index_ranges[c].first);
    }

    // As above, but returns the next value of psi.
    inline usint psiUnsafeNext(usint c, RLEVector::Iterator& iter) const
    {
      return iter.selectNext();
    }

    inline pair_type backwardSearchStep(pair_type range, usint c) const
    {
      if(this->array[c] == 0) { return EMPTY_PAIR; }
      RLEVector::Iterator iter(*(this->array[c]));

      usint start = this->index_ranges[c].first + this->number_of_sequences - 1;
      range.first = start + iter.rank(range.first, true);
      range.second = start + iter.rank(range.second);

      return range;
    }

    inline usint LF(usint index, usint c, RLEVector::Iterator& iter) const
    {
      return this->index_ranges[c].first + this->number_of_sequences + iter.rank(index) - 1;
    }

//--------------------------------------------------------------------------
//  INTERNAL STUFF
//--------------------------------------------------------------------------

    // Creates an array of iterators for every vector in this->array.
    RLEVector::Iterator** getIterators() const;
    void deleteIterators(RLEVector::Iterator** iters) const;

    inline usint getCharacter(usint pos) const
    {
      const usint* curr = &(this->text_chars[this->index_pointers[pos / this->index_rate]]);
      while(pos > this->index_ranges[*curr].second) { curr++; }
      return *curr;
    }

    void mergeEndPoints(RLCSA& index, RLCSA& increment);
    void mergeSamples(RLCSA& index, RLCSA& increment, usint* positions);

    void buildCharIndexes(usint* distribution);

    // These are not allowed.
    RLCSA();
    RLCSA(const RLCSA&);
    RLCSA& operator = (const RLCSA&);
};


// Returns the total number of characters.
usint buildRanges(usint* distribution, pair_type* index_ranges);


} // namespace CSA


#endif // RLCSA_H
