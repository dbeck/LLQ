#ifndef LLQ_SHMEM_INCLUDED
#define LLQ_SHMEM_INCLUDED

#include <cstdint>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <stdexcept>
#include <string>

#include "config.hh"
#include "no_copy.hh"
#include "no_default_construct.hh"

namespace LLQ {

  class shmem final : no_copy, no_default_construct
  {
    std::string  name_;
    size_t       size_;
    bool         writable_;
    int          fd_;

  public:
    shmem(const std::string & name, size_t size, bool writable);
    ~shmem();
    const std::string & name() const;
    size_t size() const;
    bool writable() const;
    int fd() const;
  };

  // implementation
  shmem::shmem(const std::string & name, size_t sz, bool wr)
  : no_copy("deleted"),
    no_default_construct("deleted"),
    name_{name},
    size_{sz},
    writable_{wr}
  {
    if( size_ == 0 ) throw std::invalid_argument{"size is zero"};
    if( name.at(0) != '/' ) throw std::invalid_argument{"name must start with '/'"};
    if( name.find('/',1) != std::string::npos )
      throw std::invalid_argument{"only the first character should be '/'"};

    mode_t mode = S_IRUSR;
    int oflags  = O_RDONLY;

    if( writable_ )
    {
      mode   = S_IWUSR|S_IRUSR;
      oflags = O_CREAT|O_RDWR;
    }

    fd_ = shm_open(name.c_str(), oflags, mode);
    if( fd_ < 0 ) throw std::runtime_error{"shm_open() failed"};
  }

  shmem::~shmem()
  {
    if( fd_ >= 0 ) ::close(fd_);
  }

  const std::string & shmem::name() const { return name_; }
  size_t shmem::size() const { return size_; }
  bool shmem::writable() const { return writable_; }
  int shmem::fd() const { return fd_; }
}

#endif /* end of include guard: LLQ_SHMEM_INCLUDED */
