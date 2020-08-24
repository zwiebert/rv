#pragma once

//#define USE_STD_LIST

#ifdef USE_STD_LIST
#include <list>
#include "rv/rv_timer.hh"
class RvTimer;

typedef std::list<RvTimer> RvtList;

#else


#include <stddef.h>
#include <memory>

template<class T>
struct Node {
  Node<T> *pred, *succ;
  T obj;

  Node<T>(Node<T> *head = 0) :
      pred(head), succ(head) {
  }

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
};

template<class T, class A = std::allocator<T> >
class List {
  int mLength = 0;
  Node<T> head;

public:
  List<T,A>() : head((Node<T>*)&head)


      {
  }

  class iterator {
     Node<T> *current;
  public:
     iterator(Node<T>* obj): current(obj) {};
     Node<T>* operator=(Node<T> *rhs) { return (current = rhs); }
     T* operator->() { return &current->obj; }
     bool operator==(const T *rhs) { return current == rhs; }
     //bool operator!=(const iterator rhs) { return current != rhs; }
     Node<T>* operator++() { return current = current->succ; }
     Node<T>* operator++(int n) { Node<T> * result = current; current = current->succ; return result; }
     operator Node<T>*() { return current; }
     T& operator *() { return current->obj; }
  };

  void push_back(T &obj);
  void emplace_back();
  void erase(iterator it);
  void pop_front();

  T& front() {
    return head.succ->obj;
  }

  T& back() {
    return head.pred->obj;
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

class RvTimer;
typedef List<RvTimer> RvtList;
typedef Node<RvTimer> RvtNode;




template<class T, class A> void List<T,A>::emplace_back() {

  Node<T> *node = new Node<T>(0);

  ++mLength;
  Node<T> *tail = head.pred;
  tail->succ = node;
  node->pred = tail;
  head.pred = node;
  node->succ = (Node<T> *)&head;
}

template<class T, class A> void List<T,A>::push_back(T &obj) {
  emplace_back();
  head.pred->obj = obj;
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
