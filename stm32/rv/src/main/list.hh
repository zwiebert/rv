/*
 * list.hh
 *
 *  Created on: 05.07.2019
 *      Author: bertw
 */

#ifndef MAIN_LIST_HH_
#define MAIN_LIST_HH_

template <class T>
struct Node {
public:
  bool mIsHead;
  T *pred, *succ;

  Node<T>(T* head = 0) : mIsHead(head != 0), pred(head), succ(head) {

  }

  T *getNext() {
    return succ->mIsHead ? 0 : succ;
  }

  bool isListHead() {
    return mIsHead;
  }

};

template <class T>
struct List : public Node<T> {
  int mLength = 0;

  List<T>(): Node<T>((T*)this) {

  }

  void append(T *obj) {
    ++mLength;

    T *tail = this->pred;
    tail->succ = obj;
    obj->pred = tail;

    this->pred = obj; // obj is new list tail
    obj->succ = (T*) this;

  }

  void remove(T *obj) {
    --mLength;
    if (obj->pred)
      obj->pred->succ = obj->succ;
    if (obj->succ)
      obj->succ->pred = obj->pred;
  }

  T *pop() {
    T *obj = this->succ->mIsHead ? 0 : this->succ;
    if (obj)
      remove(obj);
    return obj;
  }

  size_t length() {
    return mLength;
  }



};



#endif /* MAIN_LIST_HH_ */
