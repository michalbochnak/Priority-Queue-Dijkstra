/*pqueue.cpp*/

//
//   A priority queue specifically designed for Dijkstra's shortest
// weighted path algorithm.  Allows for storing (vertex, distance)
// pairs, with ability for both O(logN) pop and push.  Unlike 
// traditional priority queues, this version deletes an existing
// (vertex, distance) pair if a new pair is pushed with the same
// vertex --- this occurs in Dijkstra's algorithm when there exists
// a path to V, but then a better path is found and V's distance
// must be updated.
//
//   Internally, a min-heap is used along with a hash table to keep
// track of where vertices are currently positioned in the heap.
//

#include <iostream>
#include <iomanip>
#include <string>
#include <exception>
#include <stdexcept>

#include "pqueue.h"

using namespace std;


//
// Constructor:
//
// N is the capacity of the priority queue, i.e. the maximum # of vertices
// that can be stored.  This also implies that the vertex numbers range
// from 0..N-1.
//
PQueue::PQueue(int N)
{
	this->Positions = new int[N];   // array of vertex positions in queue
	this->Queue = new Elem[N];  // queue itself

	this->Capacity = N;  // we can support N vertices at most
	this->NumElements = 0;  // initially empty

							//
							// since queue is empty, set the positions of all the vertices to
							// -1, i.e. "not in queue":
							//
	for (int v = 0; v < N; ++v)
	{
		this->Positions[v] = -1;
	}
}


// 
// Destructor:
//
PQueue::~PQueue()
{
	delete[] this->Positions;
	delete[] this->Queue;
}


//
// Fill:
//
// Initializes the queue such that all N vertices are assigned
// the same distance.  This is equivalent to making N calls to
// Push(V, D):
//
//   foreach vertex v = 0..N-1
//     push(v, distance);
//
void PQueue::Fill(double distance)
{
	//
	// pre-fill the queue, assigning every vertex the same distance:
	//
	for (int v = 0; v < this->Capacity; ++v)
	{
		this->Queue[v].V = v;
		this->Queue[v].D = distance;

		this->Positions[v] = v;
	}

	this->NumElements = this->Capacity;
}


//
// Push:
//
// Inserts the given pair (vertex, distance) into the priority
// queue in ascending order by distance.  If two elements of the
// queue have the same distance D, which one comes first is 
// undefined.
//
// NOTE: if the vertex is already in the queue with distance D,
// then the existing (vertex, D) pair is removed, and the new
// (vertex, distance) pair inserted.
//
void PQueue::Push(int vertex, double distance)
{
	if (vertex < 0 || vertex >= this->Capacity)
		throw logic_error("Invalid vertex passed to PQueue::Push, must be 0..N-1");

	//
	// Is the vertex already in the queue?  If so, we need to delete the
	// existing (vertex, distance) element from queue, and then re-insert
	// with updated distance:
	//
	int position = this->Positions[vertex];

	if (position >= 0)  // vertex is currently stored in the queue:
	{
		int deletedV = this->Delete(position);

		if (deletedV != vertex)
			throw logic_error("**Internal error: vertex mismatch in PQueue::Push");
		if (this->Positions[vertex] != -1)
			throw logic_error("**Internal error: vertex position not deleted in PQueue::Push");
	}

	//
	// At this point the vertex is not in the queue, so now we do a
	// normal insert into a min heap:
	//
	this->Insert(vertex, distance);

	// success:
	return;
}


//
// PopMin:
//
// Pops (and removes) the (vertex, distance) pair at the front of
// the queue, and returns vertex.  If the queue is empty, then this
// operation is an error and so a logic_error exception is thrown
// with the error message "stack empty!".
//
int PQueue::PopMin()
{
	if (this->Empty())
		throw logic_error("stack empty!");

	int v = this->Delete(0);  // min element is at front => position 0:

	return v;
}


//
// Empty:
//
// Returns true if the queue is empty, false if not.
//
bool PQueue::Empty()
{
	return (this->NumElements == 0);
}


//
// Dump:
//
// Dumps the contents of the queue to the console; this is for
// debugging purposes.
//
void PQueue::Dump(string title)
{
	cout << ">>PQueue: " << title << endl;

	cout << "  # elements: " << this->NumElements << endl;

	if (this->Empty())  // no output:
		;
	else if (this->NumElements < 100)  // smallish, can print entire contents:
	{
		cout << std::fixed;
		cout << std::setprecision(2);

		cout << "  ";
		for (int i = 0; i < this->NumElements; ++i)
		{
			cout << "(" << this->Queue[i].V << "," << this->Queue[i].D << ") ";
		}

		cout << endl;

		cout << "  Positions: ";
		for (int v = 0; v < this->Capacity; ++v)
		{
			if (this->Positions[v] != -1)
				cout << "(" << v << "@" << this->Positions[v] << ") ";
		}

		cout << endl;
	}
	else  // Graph contains 100+ elements, so let's print a summary:
	{
		cout << std::fixed;
		cout << std::setprecision(2);

		cout << "  ";
		for (int i = 0; i < 3; ++i)
		{
			cout << "(" << this->Queue[i].V << "," << this->Queue[i].D << ") ";
		}
		cout << "... ";
		for (int i = this->NumElements - 3; i < this->NumElements; ++i)
		{
			cout << "(" << this->Queue[i].V << "," << this->Queue[i].D << ") ";
		}
		cout << endl;

		int count = 0;

		cout << "  Positions: ";
		for (int v = 0; v < this->Capacity; ++v)
		{
			if (this->Positions[v] != -1)
			{
				cout << "(" << v << "@" << this->Positions[v] << ") ";

				count++;
				if (count == 3)
					break;
			}
		}
		cout << "... ";
		int v = this->Capacity - 1;
		while (true)
		{
			if (this->Positions[v] != -1)
			{
				count--;
				if (count == 0)
					break;
			}

			v--;
		}
		for (/*empty*/; v < this->Capacity; ++v)
		{
			if (this->Positions[v] != -1)
			{
				cout << "(" << v << "@" << this->Positions[v] << ") ";

				count++;
				if (count == 3)
					break;
			}
		}
		cout << endl;

	}
}


/*************************** PRIVATE HELPER FUNCTIONS *******************************/

//
// Insert:
// 
// Inserts the given (vertex, distance) pair into the priority queue; it is
// assumed that vertex v is *not* in the queue (if it was, you must delete
// before calling Insert).  Follows standard min-heap insertion algorithm
// where you insert into last position, and then swap upwards in the tree
// to it's proper position.
//
void PQueue::Insert(int v, double d)
{
	if (v < 0 || v >= this->Capacity)
		throw logic_error("Invalid vertex passed to PQueue::Insert");
	if (this->Positions[v] != -1)
		throw logic_error("**Internal error: PQueue::Insert called while vertex is still in queue");

	//
	// TODO:
	//

	Elem temp;
	temp.V = v;
	temp.D = d;

	this->Queue[this->NumElements] = temp;

	this->Positions[v] = this->NumElements;

	this->NumElements++;	

	// check if shift is needed
	if (this->Queue[this->NumElements - 1].D < this->Queue[this->getParentIndex(this->NumElements - 1)].D) {
		shiftUp(this->NumElements - 1);
	}
}


//
// Delete:
// 
// Deletes the (vertex, distance) pair at the given position in the queue,
// where 0 <= position < # of elements.  The deleted vertex v is returned.
// Use a standard min-heap deletion algorithm where deleted element is 
// replaced by the last element, and then this element has to be swapped 
// into position --- this can be upwards or downwards (or not at all).
//
int PQueue::Delete(int position)
{
	if (this->Empty())
		throw logic_error("**Internal error: call to PQueue::Delete with an empty queue");
	if (position < 0 || position >= this->NumElements)
		throw logic_error("**Internal error: invalid position in PQueue::Delete");

	int v = -1;

	//
	// TODO:
	//

	// grab the vertex to be returned
	v = this->Queue[position].V;

	// last elements in the array
	if (position == this->NumElements - 1) {

		// clear data in the Queue array
		this->Queue[position].D = -1;
		this->Queue[position].V = -1;

		// clear data in the Positions array
		this->Positions[v] = -1;

		// decrement number of elements in the Queue
		this->NumElements--;

		return v;
	}

	// root only / only one element
	// decrement the size of the array and reset the values to -1
	if (this->NumElements == 1) {
		this->NumElements--;
		this->Positions[v] = -1;
		this->Queue[this->NumElements].D = -1;
		this->Queue[this->NumElements].V = -1;
		
		return v;
	}

	//
	// more than one element in the array
	//

	// adjust size of the Queue 
	this->NumElements--;	
	// insert the value of the last element in the array into the element to be removed
	this->Queue[position] = this->Queue[this->NumElements];
	// update position array as last element was iserted into new place
	this->Positions[this->Queue[position].V] = position;
	// update position array for last element since it not exist anymore
	this->Positions[v] = -1;
	// update values in last element ( which was moved ) to  -1's
	this->Queue[this->NumElements].V = -1;
	this->Queue[this->NumElements].D = -1;


	// if in correct spot, no need to swap, return
	if (this->inCorrectSpotwithRespectToParent(position)
		&& this->inCorrectSpotwithRespectToChild(position)) {
	
		return v;
	}

	//
	// check for the swap direction: up or down the tree
	//

	// swaps needed
	this->swap(position);


	//
	// done!
	//
	return v;
}


// function return index of smaller child
int PQueue::getSmallerChild(int position) 
{
	int leftIndex, rightIndex;

	leftIndex = (position * 2) + 1;
	rightIndex = (position * 2) + 2;

	// id dostances are equal, return te index of right child
	if (this->Queue[leftIndex].D == this->Queue[rightIndex].D) {
		return rightIndex;
	}
	// return the index of the child with graater distance
	else {
		if (this->Queue[leftIndex].D > this->Queue[rightIndex].D) {
			return leftIndex;
		}
		else
			return rightIndex;
	}

}


// checks if the node is in corrent spot with respect to parent
// specifically verifies if is <= to parent
bool PQueue::inCorrectSpotwithRespectToParent(int position) {

	// check if has parent, if so verify if distance is greater than parent distance
	if (position != 0) {
		// check if distance is greater than parent
		if (this->Queue[position].D <= this->Queue[(position - 1) / 2].D) {
			return false;	// less than parent, return false
		}
	}

	return true;
}


// checks if the node is in corrent spot with respect to child
// specifically verifies if is >= to child or children
bool PQueue::inCorrectSpotwithRespectToChild(int position) {

	// check if have at least one child
	if ((this->Queue[(position * 2) + 1].D != -1)
		|| (this->Queue[(position * 2) + 2].D != -1)) {
		// check if distance is greater than smaller child, 
		// return false if not
		if (this->Queue[position].D <= this->Queue[this->getSmallerChild(position)].D) {
			return false;
		}
	}
	
	return true;
}


// swaps the nodes in the tree
// assumes swaps are needed
void PQueue::swap(int position) {

	// need to be swapped upward
	if (!(this->inCorrectSpotwithRespectToParent(position))) {
		// swap upwards
		this->shiftUp(position);
	}

	// need to be swapped downward
	if (!(this->inCorrectSpotwithRespectToChild(position))) {
		// swap downwards
		this->shiftDown(position);
	}
}


// shifts the node down
void PQueue::shiftDown(int position) {

	int leftChildIndex, rightChildIndex, minIndex;
	Elem temp;

	leftChildIndex = this->getLeftChildIndex(position);
	rightChildIndex = this->getRightChildIndex(position);

	if (rightChildIndex >= this->NumElements) {
		if (leftChildIndex >= this->NumElements)
			return;
		else
			minIndex = leftChildIndex;
	}
	else {
		minIndex = this->getSmallerChild(position);
	}

	if (this->Queue[position].D > this->Queue[minIndex].D) {
		// swap and update Positions array
		temp = this->Queue[minIndex];
		this->Queue[minIndex] = this->Queue[position];
		this->Positions[position] = minIndex;
		this->Queue[position] = temp;
		this->Positions[minIndex] = position;
		this->shiftDown(minIndex);
	}

}


// shifts the node up
void PQueue::shiftUp(int position) {

	int parentIndex, minIndex;
	Elem temp;

	parentIndex = getParentIndex(position);


	if (this->Queue[position].D >= this->Queue[parentIndex].D) {
		return;
	}

	// swap the node up
	if (this->Queue[position].D < this->Queue[parentIndex].D) {
		// swap and update the node position
		temp = this->Queue[parentIndex];
		this->Queue[parentIndex] = this->Queue[position];
		this->Positions[this->Queue[parentIndex].V] = parentIndex;
		this->Queue[position] = temp;
		this->Positions[this->Queue[position].V] = position;
		this->shiftUp(parentIndex);
	}


}


// return index of left child 
int PQueue::getLeftChildIndex(int position) {

	return (position * 2) + 1;
}


// return index of right child 
int PQueue::getRightChildIndex(int position) {

	return (position * 2) + 2;
}


// return the index of of parent
int PQueue::getParentIndex(int position) {
	
	return (position - 1) / 2;
}