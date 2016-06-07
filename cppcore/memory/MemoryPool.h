/*
 * MemoryPool.h
 *
 *  Created on: 2016-05-07
 *      Author: Anton Kochnev
 */

#ifndef MEMORYPOOL_H_
#define MEMORYPOOL_H_

#include <cstdint>

namespace cppcore
{
  class MemoryPool
  {
  public:
    struct DebugInfo
    {
      std::size_t allocation_count = 0;
      std::size_t allocated_size = 0;

      void clear()
      {
        allocation_count = 0;
        allocated_size = 0;
      }
    };

  private:
    typedef void* Pointer;

    class Page
    {
    public:
      Pointer ptr;

    public:
      Page(std::size_t page_size)
      {
        ptr = ::operator new(page_size + sizeof(Page*));
      }

      ~Page()
      {
        ::operator delete(ptr);
      }

      Page*& prev()
      {
        return reinterpret_cast<Page*&>(ptr);
      }
    };

  public:
    MemoryPool(const MemoryPool&) = delete;
    MemoryPool& operator= (const MemoryPool&) = delete;

    MemoryPool(std::size_t page_size)
    :
      page_size_(page_size),
      offset_(page_size)
    {}

    MemoryPool(MemoryPool&& arg)
    :
      page_size_(arg.page_size_),
      curr_page_(arg.curr_page_),
      offset_(arg.offset_),
      debug_info_(arg.debug_info_)
    {
      arg.curr_page_ = nullptr;
    }

    ~MemoryPool()
    {
      clear();
    }

    void clear()
    {
      while (curr_page_)
      {
        Pointer page = curr_page_;
        curr_page_ = prev(curr_page_);
        ::operator delete(page);
      }

      debug_info_.clear();
    }

    void* allocate(std::size_t n)
    {
      ++debug_info_.allocation_count;
      debug_info_.allocated_size += n;
      const std::size_t capacity = page_size_ - offset_;

      if (n > capacity)
      {
        if (n <= page_size_)
        {
          Pointer page = new_page(n);
          prev(page) = curr_page_;
          curr_page_ = page;
          offset_ = 0;
        }
        else
        {
          Pointer page = new_page(n);

          if (curr_page_)
          {
            prev(page) = prev(curr_page_);
            prev(curr_page_) = page;
          }
          else
          {
            prev(page) = nullptr;
          }

          return ptr(page, 0);
        }
      }

      Pointer p = ptr(curr_page_, offset_);
      offset_ += n;
      return p;
    }

    const DebugInfo& get_debug_info() const
    {
      return debug_info_;
    }

  private:
    static Pointer& prev(Pointer& page)
    {
      return reinterpret_cast<Pointer&>(page);
    }

    static Pointer ptr(const Pointer page, std::size_t offset)
    {
      return (static_cast<char*>(page) + sizeof(Pointer) + offset);
    }

    static Pointer new_page(std::size_t page_size)
    {
      return ::operator new(page_size + sizeof(Pointer));
    }

  private:
    const std::size_t page_size_;
    Pointer curr_page_ = nullptr;
    std::size_t offset_;
    DebugInfo debug_info_;
  };
}

#endif /* MEMORYPOOL_H_ */
