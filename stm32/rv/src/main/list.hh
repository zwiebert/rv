#pragma once

#include <list>
#include <malloc.h>



template<class T>
class my_allocator {
public:
  typedef size_t size_type;
  typedef ptrdiff_t difference_type;
  typedef T *pointer;
  typedef const T *const_pointer;
  typedef T &reference;
  typedef const T &const_reference;
  typedef T value_type;

  my_allocator() {
  }
  my_allocator(const my_allocator&) {
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
  my_allocator<T>& operator=(const my_allocator&) {
    return *this;
  }

  size_type max_size() const {
    return size_t(-1);
  }

  template<class U>
  struct rebind {
    typedef my_allocator<U> other;
  };

  template<class U>
  my_allocator(const my_allocator<U>&) {
  }

  template<class U>
  my_allocator& operator=(const my_allocator<U>&) {
    return *this;
  }
};

template <class T> using TList = std::list<T, my_allocator<T>>;
