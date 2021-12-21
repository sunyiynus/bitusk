//
//
// For debug facility.
#ifndef BITUSK_SRC_DEBUG_H
#define BITUSK_SRC_DEBUG_H
#include <iostream>
#include <list>
#include <algorithm>
#include <vector>

#define DEBUG_OUTPUT std::cout

#define PRINT_CONTAINER(x) print_container(std::cout,x)
#define PRINT_STR(x) print_string(std::cout,x)


template<typename T>
void print_container(std::ostream& out, const T& container) {
  out << __FILE__ << ":" << __LINE__ << std::endl;
  std::for_each(container.begin(), container.end(), [&](auto& p) {
      out << p << std::endl;
      });
  out << "Total Size : "<< container.size() <<  std::endl;
}

template<>
void print_container<std::string>(std::ostream& out, const std::string& container) {
  out << __FILE__ << ":" << __LINE__ << std::endl;
  out << container << std::endl;
}

template<>
void print_container<int>(std::ostream& out, const int& container) {
  out << __FILE__ << ":" << __LINE__ << std::endl;
  out << container << std::endl;
}

template<>
void print_container<double>(std::ostream& out, const double& container) {
  out << __FILE__ << ":" << __LINE__ << std::endl;
  out << container << std::endl;
}

#endif // BITUSK_SRC_DEBUG_H
