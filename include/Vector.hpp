#ifndef VECTOR_HPP
#define VECTOR_HPP

#include<fstream>

template<typename T>
class Vector {
private:
  std::string file;
  int sizeofT = sizeof(T);
  size_t get_size() const {
     std::ifstream infile(file, std::ios::binary | std::ios::ate);
      if (!infile.is_open()) {
        throw std::runtime_error("Unable to open file");
      }
      size_t size = infile.tellg();
      return size / sizeof(T);
  }
public:
  Vector(const std::string& filename) : file(filename) {
    
  }
  void push_back(const T& value) {
    std::ofstream outfile(file, std::ios::binary | std::ios::app);
    if (!outfile.is_open()) {
      throw std::runtime_error("Unable to open file");
    }
    outfile.write(reinterpret_cast<const char*>(&value), sizeofT);
  }
  void pop_back() {
    size_t size = get_size();
    std::ofstream outfile(file, std::ios::binary);
    if (!outfile.is_open()) {
      throw std::runtime_error("Unable to open file");
    }
    for (size_t i = 0; i < size - 1; i++) {
      T value;
      std::ifstream infile(file, std::ios::binary);
      if (!infile.is_open()) {
        throw std::runtime_error("Unable to open file");
      }
      infile.seekg(i * sizeofT);
      infile.read(reinterpret_cast<char*>(&value), sizeofT);
      outfile.write(reinterpret_cast<const char*>(&value), sizeofT);
    }
  }
  T& operator[](size_t index) {
    size_t size = get_size();
    if (index >= size) {
      return -1;
    }
    T value;
    std::ifstream infile(file, std::ios::binary);
    if (!infile.is_open()) {
      return -1;
    }
    infile.seekg(index * sizeofT);
    infile.read(reinterpret_cast<char*>(&value), sizeofT);
    return value;
  }
  size_t size() const {
    return get_size();
  }
};

#endif