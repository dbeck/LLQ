#pragma once

namespace LLQ {
  
  class buffer final
  {
    int     fd_;
    size_t  size_;
    
    buffer() = delete;
    
  public:
    buffer(int fd);
    ~buffer();
  };
  
  // implementation
  buffer::buffer(int fd) {}
  buffer::~buffer() {}
}
