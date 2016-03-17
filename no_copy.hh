#ifndef LLQ_NO_COPY_INCLUDED
#define LLQ_NO_COPY_INCLUDED

namespace LLQ {
  class no_copy {
    no_copy(const no_copy &) = delete;
    no_copy & operator=(const no_copy &) = delete;
  public:
    inline no_copy(const void *) {}
  };
}

#endif /* end of include guard: LLQ_NO_COPY_INCLUDED */
