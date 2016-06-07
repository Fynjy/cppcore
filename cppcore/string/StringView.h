/*
 * StringView.h
 *
 *  Created on: 2016-05-14
 *      Author: Anton Kochnev
 */

#ifndef STRINGVIEW_H_
#define STRINGVIEW_H_

#include <string>

namespace cppcore
{
  class StringView
  {
  public:
    StringView(const char* data, std::size_t length)
    :
      data_(data),
      length_(length)
    {}

    StringView(const std::string& str)
    :
      data_(str.c_str()),
      length_(str.length())
    {}

    StringView(const std::string& str, std::size_t offset, std::size_t length)
    :
      data_(str.c_str() + offset),
      length_(length)
    {}

    const char* data() const
    {
      return data_;
    }

    const std::size_t length() const
    {
      return length_;
    }

    std::string str() const
    {
      return std::string(data_, length_);
    }

  private:
    const char* data_;
    std::size_t length_;
  };
}

#endif /* STRINGVIEW_H_ */
