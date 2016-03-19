#ifndef LLQ_LAYOUT_INCLUDED
#define LLQ_LAYOUT_INCLUDED

#include <stdexcept>
#include <string.h>

namespace LLQ {
  
  struct layout_1 {
    
    // - 4 * 64Bytes - 4 CacheLines on most systems
    struct header
    {
      uint32_t  magic_;
      uint32_t  seqno_;
      uint16_t  flags_[28+32+32+32];
      //
    } __attribute__((packed));
    
    struct area {
      char *  start_;
      size_t  size_;
      size_t  max_item_;
      size_t  act_item_;
      size_t  act_pos_;
    };
    
    size_t    n_items_;
    header *  header_;
    area      areas_[2];
    
    layout_1(char * buffer, size_t size)
    {
      int64_t area_size = (size-sizeof(header)-(2*sizeof(uint16_t)))/2;
      
      if( area_size < 1024 )
        throw std::invalid_argument{"size is too small"};
      
      if( !buffer )
        throw std::invalid_argument{"buffer argument is null"};

      // setup header
      header_ = reinterpret_cast<header *>(buffer);
      ::memset(header_, 0, sizeof(header));
      n_items_ = sizeof(header_->flags_) / sizeof(uint16_t) / 2;
      
      // setup first area
      areas_[0].start_     = buffer + sizeof(header) + (2*sizeof(uint16_t));
      areas_[0].size_      = area_size;
      areas_[0].max_item_  = area_size / n_items_;
      areas_[0].act_item_  = 0;
      areas_[0].act_pos_   = 0;
      
      // setup second area
      areas_[1].start_     = areas_[0].start_ + area_size;
      areas_[1].size_      = area_size;
      areas_[1].max_item_  = area_size / n_items_;
      areas_[1].act_item_  = 0;
      areas_[1].act_pos_   = 0;
    }
    
    void add(const void * ptr, size_t size)
    {
      int idx = (header_->seqno_)&1;
      area & a = areas_[idx];
      
      if( size > a.max_item_ )
        throw std::invalid_argument{"size is too big"};
      
      if( !ptr )
        throw std::invalid_argument{"ptr is null"};
      
      header_->flags_[2*(a.act_item_)+idx] = size;
      
      ::memcpy( a.start_+a.act_pos_, ptr, size );
      a.act_pos_ += size;
      ++(a.act_item_);
      a.max_item_ = (a.size_ - a.act_pos_) / a.act_item_;
      header_->flags_[2*(a.act_item_)+idx] = 0;
      
      if( a.act_item_ == n_items_ )
      {
        ++(header_->seqno_);
        idx           = (header_->seqno_)&1;
        a             = areas_[idx];
        a.act_item_  = 0;
        a.act_pos_   = 0;
        a.max_item_  = a.size_ / n_items_;
      }
    }
    
  };

  // implementation
}

#endif /* end of include guard: LLQ_LAYOUT_INCLUDED */
