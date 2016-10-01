#ifndef CPPCORE_MEMORYPOOLALLOCATOR_H_
#define CPPCORE_MEMORYPOOLALLOCATOR_H_

#include <limits>

#include "MemoryPool.h"

namespace cppcore
{
  template<typename T>
  class MemoryPoolAllocator
  {
  public:
    typedef std::size_t     size_type;
    typedef T*              pointer;
    typedef const T*        const_pointer;
    typedef T&              reference;
    typedef const T&        const_reference;
    typedef T               value_type;

    template <typename U>
    friend class MemoryPoolAllocator;

  public:
    template<typename U>
    struct rebind
    {
      typedef MemoryPoolAllocator<U> other;
    };

    MemoryPoolAllocator(MemoryPool& memory_pool)
    :
      memory_pool_(memory_pool)
    {}

    MemoryPoolAllocator(const MemoryPoolAllocator& a)
      :
      memory_pool_(a.memory_pool_)
    {}

    template<typename U>
    MemoryPoolAllocator(const MemoryPoolAllocator<U>& a)
      :
      memory_pool_(a.memory_pool_)
    {}

    pointer address(reference x) const
    {
      return std::__addressof(x);
    }

    const_pointer address(const_reference x) const
    {
      return std::__addressof(x);
    }

    pointer allocate(size_type n, const void* = nullptr)
    {
      return reinterpret_cast<pointer>(
        memory_pool_.allocate(n * sizeof(T)));
    }

    void deallocate(pointer, size_type)
    {}

    size_type max_size() const
    {
      return (std::numeric_limits<std::size_t>::max() / sizeof(T));
    }

    template< class U, class... Args >
    void construct(U* p, Args&&... args)
    {
      ::new((void *)p) U(std::forward<Args>(args)...);
    }

    void destroy(pointer p)
    {
      p->~T();
    }

  private:
    MemoryPool& memory_pool_;
  };

  template<typename T>
  bool operator== (const MemoryPoolAllocator<T>& arg1, const MemoryPoolAllocator<T>& arg2)
  {
    return true;
  }

  template<typename T>
  bool operator!= (const MemoryPoolAllocator<T>& arg1, const MemoryPoolAllocator<T>& arg2)
  {
    return false;
  }

  typedef MemoryPoolAllocator<void*> MemoryPoolAllocatorType;
}

#endif // CPPCORE_MEMORYPOOLALLOCATOR_H_
