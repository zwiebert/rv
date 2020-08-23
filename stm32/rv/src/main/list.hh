/*
 * list.hh
 *
 *  Created on: 05.07.2019
 *      Author: bertw
 */

#ifndef MAIN_LIST_HH_
#define MAIN_LIST_HH_

#include <stddef.h>

template<class T> class List;

template<class T>
class Node {
  friend List<T>;
private:
  bool mIsHead;
protected:
  T *pred, *succ;



public:
  Node<T>(T *head = 0) :
      mIsHead(head != 0), pred(head), succ(head) {

  }

  T* getNext() {
    return succ->mIsHead ? 0 : succ;
  }



};

template<class T>
class List: private Node<T> {
  int mLength = 0;

public:
  List<T>() :
      Node<T>((T*) this) {
  }

  void append(T *obj);
  void remove(T *obj);
  T* pop();

  T* getFirst() {
    return this->getNext();
  }

  size_t length() {
    return mLength;
  }

};

template<class T> void List<T>::append(T *obj) {
  ++mLength;

  T *tail = this->pred;
  tail->succ = obj;
  obj->pred = tail;

  this->pred = obj; // obj is new list tail
  obj->succ = (T*) this;
}

template<class T> void List<T>::remove(T *obj) {
  --mLength;
  if (obj->pred)
    obj->pred->succ = obj->succ;
  if (obj->succ)
    obj->succ->pred = obj->pred;
}

template<class T> T* List<T>::pop() {
  if (this->succ->mIsHead)
    return 0;

  T *obj = this->succ;
  remove(obj);
  return obj;
}

#endif /* MAIN_LIST_HH_ */
