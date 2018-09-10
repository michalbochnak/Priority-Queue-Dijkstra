/*pqueue.h*/

#pragma once

#include <string>
#include <exception>
#include <stdexcept>

using namespace std;


class PQueue
{
private:
  class Elem
  {
  public:
    int     V;
    double  D;

    Elem()  // default constructor:
    {
      V = -1;
      D = -1.0;
    }

    Elem(int v, double d)
    {
      V = v;
      D = d;
    }
  };

  int  *Positions;    // position of every vertex in queue (-1 if not present)
  Elem *Queue;        // actual priority queue

  int   NumElements;  // # of elements currently in queue
  int   Capacity;     // max # of vertices we can support

  void Insert(int v, double d);
  int  Delete(int position);

  // added functions
  int getSmallerChild(int position);
  bool inCorrectSpotwithRespectToParent(int position);
  bool inCorrectSpotwithRespectToChild(int position);
  void swap(int position);
  void shiftDown(int position);
  void shiftUp(int position);
  int getRightChildIndex(int position);
  int getLeftChildIndex(int position);
  int getParentIndex(int position);



public:
  PQueue(int N);  // constructor:
  ~PQueue();      // destructor:

  void Fill(double distance);  

  void Push(int vertex, double distance);
  int  PopMin();
  bool Empty();

  void Dump(string title);  // debugging output of contents:
};
