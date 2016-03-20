#ifndef LLQ_LAYOUT_INCLUDED
#define LLQ_LAYOUT_INCLUDED

#include <stdexcept>
#include <functional>
#include <string.h>
#include "item.hh"

namespace LLQ {

  template <size_t BUFFER_SIZE, uint16_t NFLAGS=28+32+32+32>
  struct layout_1 {

    // - 4 * 64Bytes - 4 CacheLines on most systems
    struct header
    {
      uint32_t  seqno_;
      uint16_t  flags_[NFLAGS];
      //
    } __attribute__((packed));

    struct area {
      char *    start_;
      uint32_t  act_item_;
      uint32_t  act_pos_;
    };

    enum {
      n_items_   = NFLAGS/2,
      area_size_ = ((BUFFER_SIZE-sizeof(header)-(2*sizeof(uint16_t)))/2),
      max_item_  = area_size_ / n_items_
    };

    header *  header_;
    area      areas_[2];

    layout_1(char * buffer)
    {
      if( area_size_ < 1024 )
        throw std::invalid_argument{"size is too small"};

      if( !buffer )
        throw std::invalid_argument{"buffer argument is null"};

      // setup header
      header_ = reinterpret_cast<header *>(buffer);

      // setup first area
      areas_[0].start_     = buffer + sizeof(header) + (2*sizeof(uint16_t));
      areas_[0].act_item_  = 0;
      areas_[0].act_pos_   = 0;

      // setup second area
      areas_[1].start_     = areas_[0].start_ + area_size_;
      areas_[1].act_item_  = 0;
      areas_[1].act_pos_   = 0;
    }

    void reset_header()
    {
      ::memset(header_, 0, sizeof(header));
    }

    uint32_t max_item() const { return max_item_; }

    void push(const void * ptr,
              size_t size)
    {
      int idx = (header_->seqno_)&1;
      area & a = areas_[idx];
      auto pos = (2*a.act_item_)+idx;

      header_->flags_[pos]   = size;
      header_->flags_[pos+2] = 0;

      ::memcpy( a.start_+a.act_pos_, ptr, size );

      a.act_pos_ += size;
      ++(a.act_item_);

      if( a.act_item_ == n_items_ )
      {
        ++(header_->seqno_);
        idx           = (header_->seqno_)&1;
        a             = areas_[idx];
        a.act_item_   = 0;
        a.act_pos_    = 0;
      }
    }

    size_t pop(std::function<bool(const void * ptr,
                                  size_t len)> fn)
    {
      size_t ret = 0;
      return ret;
    }
  };

  // implementation
}

#endif /* end of include guard: LLQ_LAYOUT_INCLUDED */
