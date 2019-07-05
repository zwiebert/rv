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

  void unlinkThis() {
    if (pred)
      pred->succ = succ;
    if (succ)
      succ->pred = pred;
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

  List<T>(): Node<T>((T*)this) {

  }

  void append(T *timer);

};



#endif /* MAIN_LIST_HH_ */
