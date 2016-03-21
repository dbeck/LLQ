#ifndef LLQ_LAYOUT_INCLUDED
#define LLQ_LAYOUT_INCLUDED

#include <stdexcept>
#include <functional>
#include <iostream>
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
      n_items_       = NFLAGS/2,
      area_size_     = ((BUFFER_SIZE-sizeof(header)-(2*sizeof(uint16_t)))/2),
      max_item_      = area_size_ / n_items_,
      first_start_   = sizeof(header) + (2*sizeof(uint16_t)),
      second_start_  = first_start_ + area_size_
    };

    header *  header_;
    area      areas_[2];

    layout_1(char * buffer);
    void reset_header();
    uint32_t max_item() const;
    void push(const void * ptr, size_t size);
    size_t pop(uint64_t first,
               std::function<bool(const void * ptr,
                                  uint16_t len,
                                  uint64_t id)> fn);
  };

  // implementation
  
  template <size_t BUFFER_SIZE, uint16_t NFLAGS>
  layout_1<BUFFER_SIZE,NFLAGS>::layout_1(char * buffer)
  {
    if( area_size_ < 1024 )
      throw std::invalid_argument{"size is too small"};
    
    if( !buffer )
      throw std::invalid_argument{"buffer argument is null"};
    
    // setup header
    header_ = reinterpret_cast<header *>(buffer);
    
    // offsets
    size_t first_offset  = first_start_;
    size_t second_offset = second_start_;
    
    // setup first area
    areas_[0].start_     = buffer + first_offset;
    areas_[0].act_item_  = 0;
    areas_[0].act_pos_   = 0;
    
    // setup second area
    areas_[1].start_     = buffer + second_offset;
    areas_[1].act_item_  = 0;
    areas_[1].act_pos_   = 0;
  }
  
  template <size_t BUFFER_SIZE, uint16_t NFLAGS>
  void
  layout_1<BUFFER_SIZE,NFLAGS>::reset_header()
  {
    ::memset(header_, 0, sizeof(header));
  }
  
  template <size_t BUFFER_SIZE, uint16_t NFLAGS>
  uint32_t
  layout_1<BUFFER_SIZE,NFLAGS>::max_item() const { return max_item_; }
  
  template <size_t BUFFER_SIZE, uint16_t NFLAGS>
  void
  layout_1<BUFFER_SIZE,NFLAGS>::push(const void * ptr,
                                     size_t size)
  {
    int idx    = (header_->seqno_)&1;
    area * ap  = areas_ + idx;
    auto pos   = (2*ap->act_item_)+idx;
    
    header_->flags_[pos]   = size;
    header_->flags_[pos+2] = 0;
    
    ::memcpy( ap->start_+ap->act_pos_, ptr, size );
    
    ap->act_pos_ += size;
    ++(ap->act_item_);
    
    if( ap->act_item_ == n_items_ )
    {
      ++(header_->seqno_);
      idx             = (header_->seqno_)&1;
      ap              = areas_ + idx;
      ap->act_item_   = 0;
      ap->act_pos_    = 0;
    }
  }
  
  template <size_t BUFFER_SIZE, uint16_t NFLAGS>
  size_t
  layout_1<BUFFER_SIZE,NFLAGS>::pop(uint64_t first,
                                    std::function<bool(const void * ptr,
                                                       uint16_t len,
                                                       uint64_t id)> fn)
  {
    auto last_seqno = header_->seqno_;
    
    // the requested id is greater than what we have
    if( (first>>16) > last_seqno ) return 0;
    // or the seqno has flipped over, than we go to pos zero
    else if ((first>>16) < last_seqno ) first = 0;
    
    int idx = last_seqno&1;
    area & a = areas_[idx];
    
    int n_found = 0;
    size_t act_pos = 0;
    int start_offset = (2*(first&0xffff));
    
    for( int i=idx; i<NFLAGS; i+=2 )
    {
      auto pos = header_->flags_[i];
      if( pos && (last_seqno == header_->seqno_) )
      {
        if( i >= start_offset )
        {
          ++n_found;
          uint64_t id = (((uint64_t)last_seqno)<<16) + (i/2) + 1;
          if( !fn(a.start_+act_pos, pos, id) )
          {
            break;
          }
        }
        act_pos += pos;
      }
      else
      {
        break;
      }
    }
    return n_found;
  }
}

#endif /* end of include guard: LLQ_LAYOUT_INCLUDED */
