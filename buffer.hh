#ifndef LLQ_BUFFER_INCLUDED
#define LLQ_BUFFER_INCLUDED

#include <cstdint>
#include <sys/types.h>
#include <sys/mman.h>
#include <stdexcept>
#include <unistd.h>

#include "config.hh"
#include "no_copy.hh"
#include "no_default_construct.hh"

namespace LLQ {

  class buffer final : no_copy, no_default_construct
  {
    size_t  size_;
    size_t  page_size_;
    bool    writable_;
    char *  buffer_;

  public:
    buffer(int fd, size_t size, bool writable);
    ~buffer();
    size_t size() const;
    size_t page_size() const;
    bool writable() const;
    char * ptr();
  };

  // implementation

  buffer::buffer(int fd, size_t sz, bool wr)
  : no_copy{"deleted"},
    no_default_construct{"deleted"},
    size_{sz},
    page_size_{0},
    writable_{wr},
    buffer_{nullptr}
  {
    auto page_size = getpagesize();
    page_size_ = page_size;

    if( fd < 0 )   throw std::invalid_argument{"fd is invalid"};
    if( sz == 0 )  throw std::invalid_argument{"size is zero"};
    if( page_size_ <= 0 ) throw std::runtime_error{"getpagesize() returns invalid value"};

    if( (sz % page_size_) != 0 )
      throw std::invalid_argument{"size must be a multiple of page_size"};

    if( sz < (page_size_ * 2) )
      throw std::invalid_argument{"size must be larger than two pages"};

    auto prot = writable_ ? (PROT_READ|PROT_WRITE) : PROT_READ;

    buffer_ = (char *)::mmap(nullptr, size_, prot, LLQ_MMAP_FLAGS, fd, 0);

    if( buffer_ == MAP_FAILED )
    {
      throw std::runtime_error{"mmap failed"};
    }
  }

  buffer::~buffer()
  {
    if( buffer_ && buffer_ != MAP_FAILED )
    {
      ::msync(buffer_, size_, MS_SYNC);
      ::munmap(buffer_, size_);
    }
  }

  size_t buffer::size() const { return size_; }
  size_t buffer::page_size() const { return page_size_; }
  bool buffer::writable() const { return writable_; }
  char * buffer::ptr() { return buffer_; }
}

#endif /* end of include guard: LLQ_BUFFER_INCLUDED */
