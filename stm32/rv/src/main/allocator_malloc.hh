#pragma once

#include <list>
#include <malloc.h>

template<class T>
class AllocatorMalloc {
public:
  typedef size_t size_type;
  typedef ptrdiff_t difference_type;
  typedef T *pointer;
  typedef const T *const_pointer;
  typedef T &reference;
  typedef const T &const_reference;
  typedef T value_type;

  AllocatorMalloc() {
  }
  AllocatorMalloc(const AllocatorMalloc&) {
  }

  pointer allocate(size_type n, const void* = 0) {
    return (T*) malloc(n * sizeof(T));
  }

  void deallocate(void *p, size_type) {
    free(p);
  }

  pointer address(reference x) const {
    return &x;
  }
  const_pointer address(const_reference x) const {
    return &x;
  }
  AllocatorMalloc<T>& operator=(const AllocatorMalloc&) {
    return *this;
  }

  size_type max_size() const {
    return size_t(-1);
  }

  template<class U>
  struct rebind {
    typedef AllocatorMalloc<U> other;
  };

  template<class U>
  AllocatorMalloc(const AllocatorMalloc<U>&) {
  }

  template<class U>
  AllocatorMalloc& operator=(const AllocatorMalloc<U>&) {
    return *this;
  }

  bool operator==(AllocatorMalloc const &a) {
    return this == &a;
  }
  bool operator!=(AllocatorMalloc const &a) {
    return !operator==(a);
  }
};

//template<class T> using TList = std::list<T, AllocatorMalloc<T>>;
//#include <string>
//typedef std::basic_string<char, std::char_traits<char>, AllocatorMalloc<char>> String;
