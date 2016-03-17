#ifndef LLQ_NO_DEFAULT_CONSTRUCT_INCLUDED
#define LLQ_NO_DEFAULT_CONSTRUCT_INCLUDED

namespace LLQ {
  class no_default_construct
  {
    no_default_construct() = delete;
  public:
    inline no_default_construct(const void *) {}
  };
}

#endif /* end of include guard: LLQ_NO_DEFAULT_CONSTRUCT_INCLUDED */
