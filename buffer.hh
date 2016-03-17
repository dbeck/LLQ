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
    void *  buffer_;

  public:
    buffer(int fd, size_t size, bool writable);
    ~buffer();
    size_t size() const;
    bool writable() const;
  };

  // implementation
  buffer::buffer(int fd, size_t sz, bool wr) :
    no_copy("deleted"),
    no_default_construct("deleted"),
    size_{sz},
    page_size_{0},
    writable_{wr},
    buffer_{nullptr}
  {
    if( fd < 0 )   throw std::invalid_argument{"fd is invalid"};
    if( sz == 0 )  throw std::invalid_argument{"size is zero"};

    auto prot = writable_ ? (PROT_WRITE | PROT_READ) : PROT_READ;
    buffer_ = mmap(nullptr, sz, prot, MAP_SHARED, fd, 0);

    if( buffer_ == MAP_FAILED ) throw std::runtime_error{"mmap failed"};
  }

  buffer::~buffer() {}
  size_t buffer::size() const { return size_; }
  bool buffer::writable() const { return writable_; }
}

#endif /* end of include guard: LLQ_BUFFER_INCLUDED */
