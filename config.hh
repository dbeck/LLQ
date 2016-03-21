#ifndef LLQ_CONFIG_INCLUDED
#define LLQ_CONFIG_INCLUDED

#ifdef __APPLE__
#include <sys/time.h>
#include <sys/posix_shm.h>
#define LLQ_SHMEM_NAME_MAX_LEN PSHMNAMLEN 
#define LLQ_MMAP_FLAGS  MAP_SHARED|MAP_NOCACHE
//
#elif __linux__
#include <unistd.h>
#include <dirent.h>
#define LLQ_SHMEM_NAME_MAX_LEN NAME_MAX
#define LLQ_MMAP_FLAGS MAP_SHARED|MAP_LOCKED
//
#else
#error "only mac OSX and Linuxis supported at the moment"
#endif

#endif /* end of include guard: LLQ_CONFIG_INCLUDED */
