#ifndef RLCSA_BUILDER_H
#define RLCSA_BUILDER_H

#include "rlcsa.h"


namespace CSA
{


class RLCSABuilder
{
  public:
    RLCSABuilder(usint _block_size, usint _sample_rate, usint _buffer_size, usint _threads = 1);
    ~RLCSABuilder();

    void insertSequence(char* sequence, usint length, bool delete_sequence);

    // Use this if you have already built an index for the file.
    void insertFromFile(const std::string& base_name);

    // User must free the index. Builder no longer contains it.
    RLCSA* getRLCSA();

    // User must free the BWT. length becomes the length of BWT.
    char* getBWT(usint& length);

    bool isOk();

    // These times are not reset with the rest of the builder.
    double getBuildTime();
    double getSearchTime();
    double getSortTime();
    double getMergeTime();

  private:
    RLCSA* index;

    usint block_size;
    usint sample_rate;
    usint buffer_size;

    usint threads;

    uchar* buffer;
    usint chars;

    bool ok;

    double build_time;
    double search_time;
    double sort_time;
    double merge_time;

    void flush();
    void addRLCSA(RLCSA* increment, uchar* sequence, usint length, bool delete_sequence);
    void reset();

    // These are not allowed.
    RLCSABuilder();
    RLCSABuilder(const RLCSABuilder&);
    RLCSABuilder& operator = (const RLCSABuilder&);
};


} // namespace CSA


#endif // RLCSA_BUILDER_H
