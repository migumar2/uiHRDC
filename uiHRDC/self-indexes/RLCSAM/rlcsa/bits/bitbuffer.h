#ifndef BITBUFFER_H
#define BITBUFFER_H

#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>

#include "../misc/definitions.h"


namespace CSA
{


template<class Data>
class GenericReadBuffer
{
  public:
    GenericReadBuffer(std::ifstream& file, usint words) :
      size(words),
      item_bits(1),
      items(0),
      free_buffer(true)
    {
      Data* buffer = new Data[this->size];
      memset(buffer, 0, this->size * sizeof(Data));
      file.read((char*)buffer, this->size * sizeof(Data));
      this->data = buffer;
      this->reset();
    }

    GenericReadBuffer(std::ifstream& file, usint _items, usint item_size) :
      item_bits(item_size),
      items(_items),
      free_buffer(true)
    {
      this->size = bitsToData(this->items * this->item_bits);
      Data* buffer = new Data[this->size];
      memset(buffer, 0, this->size * sizeof(Data));
      file.read((char*)buffer, this->size * sizeof(Data));
      this->data = buffer;
      this->reset();
    }

    // This version does not delete the data when deleted.
    GenericReadBuffer(const Data* buffer, usint words) :
      size(words),
      item_bits(1),
      items(0),
      free_buffer(false)
    {
      this->data = buffer;
      this->reset();
    }

    // This version does not delete the data when deleted.
    GenericReadBuffer(const Data* buffer, usint _items, usint item_size) :
      item_bits(item_size),
      items(_items),
      free_buffer(false)
    {
      this->size = bitsToData(this->items * this->item_bits);
      this->data = buffer;
      this->reset();
    }

    // This version does not delete the data when deleted.
    GenericReadBuffer(const GenericReadBuffer& original) :
      data(original.data),
      size(original.size),
      item_bits(original.item_bits),
      items(original.items),
      free_buffer(false)
    {
      this->reset();
    }

    ~GenericReadBuffer()
    {
      if(this->free_buffer)
      {
        delete[] this->data;
      }
    }

//--------------------------------------------------------------------------

    void claimData()
    {
      this->free_buffer = true;
    }

    void writeBuffer(std::ofstream& file) const
    {
      file.write((const char*)this->data, this->size * sizeof(Data));
    }

    // The buffer will no longer own the data.
    void moveBuffer(const Data* buffer)
    {
      if(this->free_buffer)
      {
        delete[] this->data;
      }
      this->free_buffer = false;

      this->data = buffer;
      this->reset();
    }

    usint reportSize() const
    {
      usint bytes = sizeof(*this);
      if(this->free_buffer) { bytes += this->size * sizeof(Data); }
      return bytes;
    }

//--------------------------------------------------------------------------

    inline void reset()
    {
      this->pos = 0;
      this->bits = DATA_BITS;
      this->current = 0;
    }

    inline void skipBits(usint count)
    {
      if(count < this->bits)
      {
        this->bits -= count;
        return;
      }

      count -= this->bits;
      this->pos += 1 + count / DATA_BITS;
      this->bits = DATA_BITS - count % DATA_BITS;
    }

//--------------------------------------------------------------------------

    inline usint bitsLeft() const
    {
      return this->bits + DATA_BITS * (this->size - this->pos - 1);
    }

    // Returns nonzero if bit is 1
    inline usint readBit()
    {
      this->bits--;
      usint bit = this->data[this->pos] & ((usint)1 << this->bits);

      if(this->bits == 0) { this->pos++; this->bits = DATA_BITS; }

      return bit;
    }

    inline usint readBits(usint count)
    {
      usint value = 0;

      while(count >= this->bits)
      {
        count -= this->bits;
        value |= HIGHER(GET(this->data[this->pos], this->bits), count);
        this->pos++; this->bits = DATA_BITS;
      }
      if(count > 0)
      {
        this->bits -= count;
        value |= GET(LOWER(this->data[this->pos], this->bits), count);
      }

      return value;
    }

//--------------------------------------------------------------------------

    /*
      These operations work on fixed-size items. No sanity checks are made
      for parameter values.
    */

    inline usint getItemSize() const
    {
      return this->item_bits;
    }

    inline void goToItem(usint item)
    {
      usint b = item * this->item_bits;
      this->pos = b / DATA_BITS;
      this->bits = DATA_BITS - b % DATA_BITS;
      this->current = item;
    }

    inline usint readItem()
    {
      this->current++;
      return this->readBits(this->item_bits);
    }

    inline usint readItem(usint item)
    {
      this->goToItem(item);
      return this->readItem();
    }

    inline usint readFirstItem()
    {
      return this->readItem(0);
    }

    inline usint readItemConst(usint item) const
    {
      usint b = item * this->item_bits;
      usint p = b / DATA_BITS;
      b = DATA_BITS - b % DATA_BITS;

      usint c = this->item_bits;
      usint value = 0;

      while(c >= b)
      {
        c -= b;
        value |= HIGHER(GET(this->data[p], b), c);
        p++; b = DATA_BITS;
      }
      if(c > 0)
      {
        b -= c;
        value |= GET(LOWER(this->data[p], b), c);
      }

      return value;
    }

    inline bool hasNextItem() const
    {
      return (this->current < this->items);
    }

    inline void skipItem()
    {
      this->skipBits(this->item_bits);
      this->current++;
    }

//--------------------------------------------------------------------------

    /*
      Delta coding for positive integers
    */

    inline usint readDeltaCode()
    {
      usint len = 0;
      while(this->readBit() == 0) { len++; }

      usint temp = (((usint)1 << len) | this->readBits(len)) - 1;
      temp = ((usint)1 << temp) | this->readBits(temp);
      return temp;
    }

//--------------------------------------------------------------------------

  private:
    const Data* data;
    usint size, item_bits, items;
    bool  free_buffer;

    // Iterator data
    usint pos, bits, current;

    const static usint DATA_BITS = sizeof(Data) * CHAR_BIT;

    inline static usint bitsToData(usint _bits) { return (_bits + DATA_BITS - 1) / DATA_BITS; }

    // These are not allowed.
    GenericReadBuffer();
    GenericReadBuffer& operator = (const GenericReadBuffer&);
};


//--------------------------------------------------------------------------


template<class Data>
class GenericWriteBuffer
{
  public:
    GenericWriteBuffer(usint words) :
      size(words),
      item_bits(1),
      items(0),
      free_buffer(true)
    {
      this->data = new Data[words];
      memset(this->data, 0, this->size * sizeof(Data));
      this->reset();
    }

    GenericWriteBuffer(usint _items, usint item_size) :
      item_bits(item_size),
      items(_items),
      free_buffer(true)
    {
      this->size = bitsToData(this->items * this->item_bits);
      this->data = new Data[this->size];
      memset(this->data, 0, this->size * sizeof(Data));
      this->reset();
    }

    // This version does not delete the data when deleted.
    GenericWriteBuffer(Data* buffer, usint words) :
      size(words),
      item_bits(1),
      items(0),
      free_buffer(false)
    {
      this->data = buffer;
      this->reset();
    }

    // This version does not delete the data when deleted.
    GenericWriteBuffer(Data* buffer, usint _items, usint item_size) :
      item_bits(item_size),
      items(_items),
      free_buffer(false)
    {
      this->size = bitsToData(this->items * this->item_bits);
      this->data = buffer;
      this->reset();
    }

    ~GenericWriteBuffer()
    {
      if(this->free_buffer)
      {
        delete[] this->data;
      }
    }

//--------------------------------------------------------------------------

    // This transfers the ownership of the data to the read buffer.
    GenericReadBuffer<Data>* getReadBuffer()
    {
      GenericReadBuffer<Data>* buffer;
      if(this->items > 0)
      {
        buffer = new GenericReadBuffer<Data>(this->data, this->items, this->item_bits);
      }
      else
      {
        buffer = new GenericReadBuffer<Data>(this->data, this->size);
      }

      if(this->free_buffer)
      {
        buffer->claimData();
        this->free_buffer = false;
      }

      return buffer;
    }

    void writeBuffer(std::ofstream& file) const
    {
      file.write((char*)this->data, this->size * sizeof(Data));
    }

    // The buffer will no longer own the data.
    void moveBuffer(Data* buffer)
    {
      if(this->free_buffer)
      {
        delete[] this->data;
      }
      this->free_buffer = false;

      this->data = buffer;
      this->reset();
    }

    usint reportSize() const
    {
      usint bytes = sizeof(*this);
      if(this->free_buffer) { bytes += this->size * sizeof(Data); }
      return bytes;
    }

//--------------------------------------------------------------------------

    inline void reset()
    {
      this->pos = 0;
      this->bits = DATA_BITS;
      this->current = 0;
    }

    inline void skipBits(usint count)
    {
      if(count < this->bits)
      {
        this->bits -= count;
        return;
      }

      count -= this->bits;
      this->pos += 1 + count / DATA_BITS;
      this->bits = DATA_BITS - count % DATA_BITS;
    }

//--------------------------------------------------------------------------

    inline usint bitsLeft() const
    {
      return this->bits + DATA_BITS * (this->size - this->pos - 1);
    }

    inline void writeBits(usint value, usint count)
    {
      while(count >= this->bits)
      {
        count -= this->bits;
        this->data[this->pos] |= GET(LOWER(value, count), this->bits);
        this->pos++; this->bits = DATA_BITS;
      }
      if(count > 0)
      {
        this->bits -= count;
        this->data[this->pos] |= HIGHER(GET(value, count), this->bits);
      }
    }

//--------------------------------------------------------------------------

    /*
      These operations work on fixed-size items.
    */

    inline usint getItemSize() const
    {
      return this->item_bits;
    }

    inline void goToItem(usint item)
    {
      usint b = item * this->item_bits;
      this->pos = b / DATA_BITS;
      this->bits = DATA_BITS - b % DATA_BITS;
      this->current = item;
    }

    inline bool hasNextItem() const
    {
      return (this->current < this->items);
    }

    inline void writeItem(usint item)
    {
      this->writeBits(item, this->item_bits);
      this->current++;
    }

    inline void skipItem()
    {
      this->skipBits(this->item_bits);
      this->current++;
    }

//--------------------------------------------------------------------------

    /*
      Delta coding for positive integers
    */

    inline bool canDeltaCode(usint value) const
    {
      return this->deltaCodeLength(value) <= this->bitsLeft();
    }

    inline usint deltaCodeLength(usint value) const
    {
      usint len = length(value);
      usint llen = length(len);
      return (len + llen + llen - 2);
    }

    // This version returns false if there is no space left for the encoding.
    inline bool writeDeltaCode(usint value)
    {
      usint len = length(value);
      usint llen = length(len);

      if(len + llen + llen - 2 > this->bitsLeft()) { return false; }

      // this->writeBits(0, llen - 1); // Now included in the next writeBits()
      this->writeBits(len, llen + llen - 1);
      this->writeBits(value, len - 1);
      return true;
    }

    // This version assumes the code fits into the buffer.
    inline void writeDeltaCodeDirect(usint value)
    {
      usint len = length(value);
      usint llen = length(len);

      // this->writeBits(0, llen - 1); // Now included in the next writeBits()
      this->writeBits(len, llen + llen - 1);
      this->writeBits(value, len - 1);
    }

    // We assume the code fits into usint:
    //  32-bit:  value < 2^24
    //  64-bit:  value < 2^54
    inline void writeDeltaCodeFast(usint value)
    {
      usint len = length(value);

      value ^= ((usint)1 << (len - 1));
      this->writeBits((len << (len - 1)) | value, len + 2 * length(len) - 2);
    }

//--------------------------------------------------------------------------

  private:
    Data* data;
    usint size, item_bits, items;
    bool free_buffer;

    // Iterator data
    usint pos, bits, current;

    const static usint DATA_BITS = sizeof(Data) * CHAR_BIT;

    inline static usint bitsToData(usint _bits) { return (_bits + DATA_BITS - 1) / DATA_BITS; }

    // These are not allowed.
    GenericWriteBuffer();
    GenericWriteBuffer(const GenericWriteBuffer&);
    GenericWriteBuffer& operator = (const GenericWriteBuffer&);
};


//--------------------------------------------------------------------------


typedef GenericWriteBuffer<usint> WriteBuffer;
typedef GenericReadBuffer<usint>  ReadBuffer;


} // namespace CSA


#endif // BITBUFFER_H
