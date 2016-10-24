#include "Serialization.h"

namespace cppcore
{
  template<>
  void write(std::ostream& os, const std::string& v)
  {
    write(os, v.length());
    os.write(v.c_str(), v.length());
  }

  template<>
  void read(std::istream& is, std::string& v)
  {
    std::size_t sz = 0;
    read(is, sz);
    v.resize(sz);
    is.read(&*v.begin(), sz);
  }
}
