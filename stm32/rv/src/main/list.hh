#pragma once

#define USE_STD_LIST

#ifdef USE_STD_LIST
#include <list>
#include "rv/rv_timer.hh"
class RvTimer;

typedef std::list<RvTimer> RvtList;

#else


#include <stddef.h>

template<class T> class List;

template<class T>
class Node {
  friend List<T>;
protected:
  T *pred, *succ;



public:
  Node<T>(T *head = 0) :
      pred(head), succ(head) {
  }

};

template<class T>
class List {
  int mLength = 0;
  Node<T> head;

public:
  List<T>() : head((T*)&head)


      {
  }

  void push_back(T *obj);
  void erase(T *obj);
  T* pop_front();

  T* front() {
    return head.succ;
  }

  T* back() {
    return head.pred;
  }


  size_t size() {
    return mLength;
  }
  bool empty() { return head.succ == head.pred; }

  class iterator {
     T *current;
  public:
     iterator(T* obj): current(obj) {};
     T* operator=(T *rhs) { return (current = rhs); }
     T* operator->() { return current; }
     bool operator==(const T *rhs) { return current == rhs; }
     //bool operator!=(const iterator rhs) { return current != rhs; }
     T* operator++() { return current = current->succ; }
     operator T*() { return current; }
  };

  iterator begin() {
    return head.succ;
  }

  iterator end() {
    return (T*)&head;
  }

};


class RvTimer;

typedef List<RvTimer> RvtList;


template<class T> void List<T>::push_back(T *obj) {
  ++mLength;

  T *tail = head.pred;
  tail->succ = obj;
  obj->pred = tail;

  head.pred = obj; // obj is new list tail
  obj->succ = (T*) &head;
}

template<class T> void List<T>::erase(T *obj) {
  --mLength;
  if (obj->pred)
    obj->pred->succ = obj->succ;
  if (obj->succ)
    obj->succ->pred = obj->pred;
}

template<class T> T* List<T>::pop_front() {
  precond(!empty());

  T *obj = head.succ;
  erase(obj);
  return obj;
}
#endif
