#ifndef COMMON_HPP
#define COMMON_HPP

template <typename T>
class NotEqual {
 public:
  bool operator!=(const T& a) { return !(*static_cast<T*>(this) == a); }
};

#endif  // COMMON_HPP
