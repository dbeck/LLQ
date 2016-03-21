#ifndef LLQ_SHMEM_INCLUDED
#define LLQ_SHMEM_INCLUDED

// OS specifics
#include "config.hh"

// C
#include <fcntl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>

// C++
#include <cstdint>
#include <stdexcept>
#include <string>

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
    static bool destroy(const std::string & name);
  };

  // implementation
  
  shmem::shmem(const std::string & name, size_t sz, bool wr)
  : no_copy{"deleted"},
    no_default_construct{"deleted"},
    name_{name},
    size_{sz},
    writable_{wr}
  {
    if( size_ == 0 )        throw std::invalid_argument{"size is zero"};
    if( name.empty() )      throw std::invalid_argument{"name must be non-empty"};
    if( name.at(0) != '/' ) throw std::invalid_argument{"name must start with '/'"};
    
    if( name.find('/',1) != std::string::npos )
      throw std::invalid_argument{"only the first character should be '/'"};
    
    if( name.size() > LLQ_SHMEM_NAME_MAX_LEN )
      throw std::invalid_argument{"name is too long for shm_open"};

    mode_t mode = S_IRUSR;
    int oflags  = O_RDONLY;
    
    bool needs_ftruncate = false;

    if( writable_ )
    {
      // try open in read-only mode first and determine if ftruncate will be needed or not
      fd_ = ::shm_open(name.c_str(), oflags, mode);
      if( fd_ < 0 )
      {
        // if the shared memory is not yet there, we will need to initialize it
        needs_ftruncate = true;
      }
      else
      {
        ::close(fd_);
        fd_ = -1;
      }
      
      oflags = O_CREAT|O_RDWR;
      mode   = S_IWUSR|S_IRUSR;
    }

    fd_ = ::shm_open(name.c_str(), oflags, mode);
    if( fd_ < 0 )
    {
      throw std::runtime_error{"shm_open() failed"};
    }
    
    // needs ftruncate to set the size and initialize the shared memory region
    if( needs_ftruncate && ::ftruncate(fd_, size_) < 0 )
    {
      throw std::runtime_error{"ftruncate() failed"};
    }
  }

  shmem::~shmem()
  {
    if( fd_ >= 0 ) ::close(fd_);
  }

  const std::string & shmem::name() const { return name_; }
  size_t shmem::size() const { return size_; }
  bool shmem::writable() const { return writable_; }
  int shmem::fd() const { return fd_; }

  bool shmem::destroy(const std::string & name)
  {
    if( name.empty() ) throw std::invalid_argument{"name must be non-empty"};
    if( name.at(0) != '/' ) throw std::invalid_argument{"name must start with '/'"};
    if( name.find('/',1) != std::string::npos )
      throw std::invalid_argument{"only the first character should be '/'"};
    return 0 == shm_unlink(name.c_str());
  }
}

#endif /* end of include guard: LLQ_SHMEM_INCLUDED */
