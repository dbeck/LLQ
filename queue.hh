#ifndef LLQ_QUEUE_INCLUDED
#define LLQ_QUEUE_INCLUDED

#include <cstdint>
#include <stdexcept>
#include <string>
#include <memory>

#include "config.hh"
#include "no_copy.hh"
#include "no_default_construct.hh"
#include "shmem.hh"
#include "item.hh"

namespace LLQ {
  
  template <typename DEV>
  class queue final : no_copy, no_default_construct
  {
    
    static const uint64_t header_magic = 0xDeadBeefFeedDead;
    
    struct header {
      uint64_t  magic_;
      uint64_t  seqno_;
      uint64_t  writer_bits_[2]; // placeholder
    };
    
    struct reader_state {
      std::unique_ptr<item[]> items_;
    };
    
    struct writer_state {
      int TODO_;
    };

    buffer        buffer_;
    reader_state  reader_state_;
    writer_state  writer_state_;
    
  public:
    queue(DEV & dev);
    ~queue();
    
    void push(item i);
    size_t pop();
    item * items();
  };

  // implementation
  
  template <typename DEV>
  queue<DEV>::queue(DEV & dev)
  : no_copy{"deleted"},
    no_default_construct{"deleted"},
    buffer_{dev.fd(), dev.size(), dev.writable()}
  {
    // TODO:
    // setup queue structure
    auto psz = buffer_.page_size();
    auto npages = buffer_.size() / psz;
    auto pbytes = (63+npages)/8;
    auto headsz = 16;
    auto head_pages = ((psz-1)+pbytes+headsz)/psz;
    auto rem_pages = npages - head_pages;
    
    // initialize reader and writer states
  }
  
  template <typename DEV>
  queue<DEV>::~queue()
  {
  }
  
  template <typename DEV>
  void
  queue<DEV>::push(item i)
  {
    // TODO
  }

  template <typename DEV>
  size_t
  queue<DEV>::pop()
  {
    // TODO
    return 0;
  }
  
  template <typename DEV>
  item *
  queue<DEV>::items()
  {
    return reader_state_.items_.get();
  }
}

#endif /* end of include guard: LLQ_QUEUE_INCLUDED */
