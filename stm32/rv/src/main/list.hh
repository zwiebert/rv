#pragma once

#define USE_STD_LIST

#ifdef USE_STD_LIST
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
    T *t = (T*) malloc(n * sizeof(T));
    return t;
  }

  void deallocate(void *p, size_type) {
    if (p) {
      free(p);
    }
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
  void construct(pointer p, const T &val) {
    new ((T*) p) T(val);
  }
  void destroy(pointer p) {
    p->~T();
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


#else


#include <stddef.h>
#include <memory>
#include <type_traits>

//#define USE_TRAITS_NODE

template <class T> struct AllocTemplate {

  //using value_type = T;
  //using pointer = T *;
  typedef size_t    size_type;
  typedef ptrdiff_t difference_type;
  typedef T*        pointer;
  typedef const T*  const_pointer;
  typedef T&        reference;
  typedef const T&  const_reference;
  typedef T         value_type;

  template <class U> struct rebind { typedef AllocTemplate<U> other; };

};

template <class T>
class my_allocator
{
public:
  typedef size_t    size_type;
  typedef ptrdiff_t difference_type;
  typedef T*        pointer;
  typedef const T*  const_pointer;
  typedef T&        reference;
  typedef const T&  const_reference;
  typedef T         value_type;

  my_allocator() {}
  my_allocator(const my_allocator&) {}



  pointer   allocate(size_type n, const void * = 0) {
              T* t = (T*) malloc(n * sizeof(T));
              return t;
            }

  void      deallocate(void* p, size_type) {
              if (p) {
                free(p);
              }
            }

  pointer           address(reference x) const { return &x; }
  const_pointer     address(const_reference x) const { return &x; }
  my_allocator<T>&  operator=(const my_allocator&) { return *this; }
  void              construct(pointer p, const T& val)
                    { new ((T*) p) T(val); }
  void              destroy(pointer p) { p->~T(); }

  size_type         max_size() const { return size_t(-1); }

  template <class U>
  struct rebind { typedef my_allocator<U> other; };

  template <class U>
  my_allocator(const my_allocator<U>&) {}

  template <class U>
  my_allocator& operator=(const my_allocator<U>&) { return *this; }
};




#if 0
template <typename T>
struct ArenaAllocator
{
    template <typename U> friend struct ArenaAllocator;

    using value_type = T;
    using pointer = T *;

    using propagate_on_container_copy_assignment = std::true_type;
    using propagate_on_container_move_assignment = std::true_type;
    using propagate_on_container_swap = std::true_type;

    explicit ArenaAllocator(Arena * a) : arena(a) {}

    template <typename U>
    ArenaAllocator(ArenaAllocator<U> const & rhs) : arena(rhs.arena) {}

    pointer allocate(std::size_t n)
    {
        return static_cast<pointer>(arena->allocate(n * sizeof(T), alignof(T)));
    }

    void deallocate(pointer p, std::size_t n)
    {
        arena->deallocate(p, n * sizeof(T));
    }

    template <typename U>
    bool operator==(ArenaAllocator<U> const & rhs) const
    {
        return arena == rhs.arena;
    }

    template <typename U>
    bool operator!=(ArenaAllocator<U> const & rhs) const
    {
        return arena != rhs.arena;
    }

private:
    Arena * arena;
};
#endif




template<typename T>
struct Node {
#ifdef USE_TRAITS_NODE
  using Alloc = my_allocator<T>;
  using NodeTraits = std::allocator_traits<Alloc>::rebind_traits<Node>;
  typename NodeTraits::pointer pred;
  typename NodeTraits::pointer succ;

  typename std::aligned_storage<sizeof(T), alignof(T)>::type storage;
#else
  Node<T> *pred, *succ;
  typename std::aligned_storage<sizeof(T), alignof(T)>::type storage;
  //T obj;
#endif

#ifdef USE_MALLOC_IN_NEW_NODE
void* operator new(size_t nbytes) {
  return malloc(nbytes);
}

void* operator new[](size_t nbytes) {
  return malloc(nbytes);
}

void operator delete(void *p) {
  free(p);
}
#endif
   T& getObj() { return *reinterpret_cast<T *>(&storage); }
};




template<class T, class A = std::allocator<T> >
class List {
  int mLength = 0;
  Node<T> head;

private:

public:
  List<T, A>() {
    head.pred = head.succ = (Node<T>*) &head;
  }

  class iterator {
     Node<T> *current;
  public:
     iterator(Node<T>* obj): current(obj) {};
     Node<T>* operator=(Node<T> *rhs) { return (current = rhs); }
     T* operator->() { return &current->getObj(); }
     bool operator==(const T *rhs) { return current == rhs; }
     //bool operator!=(const iterator rhs) { return current != rhs; }
     Node<T>* operator++() { return current = current->succ; }
     Node<T>* operator++(int n) { Node<T> * result = current; current = current->succ; return result; }
     operator Node<T>*() { return current; }
     T& operator *() { return current->getObj(); }
  };

  void push_back(T &obj);
  void emplace_back();
  void erase(iterator it);
  void pop_front();

  T& front() {
    return head.succ->getObj();
  }

  T& back() {
    return head.pred->getObj();
  }


  size_t size() {
    return mLength;
  }
  bool empty() { return head.succ == head.pred; }



  iterator begin() {
    return head.succ;
  }

  iterator end() {
    return (Node<T>*)&head;
  }

};

#include <new>

//class RvTimer;
//typedef List<RvTimer> RvtList;
//typedef Node<RvTimer> RvtNode;




template<class T, class A> void List<T,A>::emplace_back() {
  using Alloc = my_allocator<T>;
  Alloc a;

  Node<T> *node = (Node<T> *)a.allocate(sizeof (Node<T>));
  a.construct(&node->getObj(), T());

  ++mLength;
  Node<T> *tail = head.pred;
  tail->succ = node;
  node->pred = tail;
  head.pred = node;
  node->succ = (Node<T> *)&head;
}

template<class T, class A> void List<T,A>::push_back(T &obj) {
  emplace_back();
  head.pred->getObj() = obj;
}

template<class T, class A> void List<T,A>::erase(iterator it) {
  --mLength;
  Node<T> *obj = it;

  if (obj->pred)
    obj->pred->succ = obj->succ;
  if (obj->succ)
    obj->succ->pred = obj->pred;

  delete(obj);
}

template<class T, class A> void List<T,A>::pop_front() {
  precond(!empty());

  erase(begin());
}
#endif
