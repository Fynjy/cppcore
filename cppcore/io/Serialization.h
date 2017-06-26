#ifndef MLCORE_IO_SERIALIZATION_H_
#define MLCORE_IO_SERIALIZATION_H_

#include <istream>
#include <ostream>
#include <string>
#include <vector>

namespace cppcore
{
  template<typename T>
  inline void write(std::ostream& os, const T& v)
  {
    os.write(reinterpret_cast<const char*>(&v), sizeof(T));
  }

  template<typename T>
  inline void read(std::istream& is, T& v)
  {
    is.read(reinterpret_cast<char*>(&v), sizeof(T));
  }

  template<typename T>
  inline void write_vector_pod(std::ostream& os, const std::vector<T>& v)
  {
    write(os, v.size());
    os.write(reinterpret_cast<const char*>(&v[0]), sizeof(T) * v.size());
  }

  template<typename T>
  inline void read_vector_pod(std::istream& is, std::vector<T>& v)
  {
    std::size_t sz = 0;
    read(is, sz);
    v.resize(sz);
    is.read(reinterpret_cast<char*>(&v[0]), sizeof(T) * sz);
  }

  template<>
  inline void write(std::ostream& os, const std::string& v)
  {
    write(os, v.length());
    os.write(v.c_str(), v.length());
  }

  template<>
  inline void read(std::istream& is, std::string& v)
  {
    std::size_t sz = 0;
    read(is, sz);
    v.resize(sz);
    is.read(&*v.begin(), sz);
  }
}

#endif /* MLCORE_IO_SERIALIZATION_H_ */
