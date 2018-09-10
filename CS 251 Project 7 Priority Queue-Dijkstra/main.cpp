/*main.cpp*/

//
// Test program for Dijkstra-specific priority queue.
//

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <random>
#include <utility>
#include <set>
#include <algorithm>
#include <limits>

#include "pqueue.h"

using namespace std;

#define _CRT_SECURE_NO_WARNINGS  // ignore C-related security warnings in Visual Studio:


//
// StressTest1:
//
// The idea is to push and pop with a large # of vertices to
// reveal inefficient solutions.  This test just pushes and
// then pops them all.
//
int StressTest1(PQueue& pq, int N)
{
	int distance = N;
	int ops = 0;

	//
	// let's insert all the vertices, with successive vertices
	// having smaller distances (and so moving to the front of
	// the queue):
	//
	cout << "   ";

	for (int v = 0; v < N; ++v)
	{
		if (v < 3)
		{
			cout << "(" << v << "," << distance << ") ";
		}
		else if (v == 3)
		{
			cout << "... ";
		}
		else if (v > N - 4)
		{
			cout << "(" << v << "," << distance << ") ";
		}

		pq.Push(v, distance);
		distance--;

		ops++;
	}

	cout << endl;

	//
	// now let's pop them all and make sure we get the
	// correct distance:
	//
	int expectedV = N - 1;

	pq.Dump("reversed:");

	for (int i = 0; i < N; ++i)
	{
		int v = pq.PopMin();

		if (v != expectedV)
		{
			cout << "**Error: was expecting vertex " << expectedV
				<< " but popped " << v << endl;
			return -1;
		}

		expectedV--;
		ops++;
	}

	//
	// success!
	//
	return ops;
}


//
// StressTest2:
//
// A more random mixture of pushes and pops:
//
int StressTest2(PQueue& pq, int N)
{
	int ops = 0;

	const int MAX = numeric_limits<int>::max();

	uniform_int_distribution<int> dis(1, MAX);
	mt19937 gen;

	//
	// generate N random, distinct distances:
	//
	vector<pair<int, int>> randomPairs;
	set<int> S;

	for (int v = 0; v < N; ++v)
	{
		int d = dis(gen);  // gen a random distance:

		while (S.find(d) != S.end())  // we need a unique distance:
			d = dis(gen);

		S.insert(d);

		pair<int, int> p(v, d);
		randomPairs.push_back(p);
	}

	cout << "   ";

	for (int i = 0; i < 3; ++i)
	{
		cout << "(" << randomPairs[i].first << "," << randomPairs[i].second << ") ";
	}
	cout << "... ";
	for (int i = N - 3; i < N; ++i)
	{
		cout << "(" << randomPairs[i].first << "," << randomPairs[i].second << ") ";
	}

	cout << endl;

	//
	// now insert the pairs into the priority queue:
	//
	for (pair<int, int> p : randomPairs)
	{
		pq.Push(p.first, p.second);

		ops++;
	}

	//
	// sort the pairs so we know what to expect when we pop:
	//
	sort(randomPairs.begin(), randomPairs.end(),
		[](pair<int, int>& p1, pair<int, int>& p2)
	{
		if (p1.second < p2.second)  // sort by distance, descending:
			return true;
		else
			return false;
	}
	);

	//
	// now let's pop them all and make sure we get the
	// correct distance:
	//
	pq.Dump("contents:");

	for (int i = 0; i < N; ++i)
	{
		int v = pq.PopMin();
		int expectedV = randomPairs[i].first;

		if (v != expectedV)
		{
			cout << "**Error: was expecting vertex " << expectedV
				<< " but popped " << v << endl;
			return -1;
		}

		ops++;
	}

	//
	// success!
	//
	return ops;
}



//#define VS


int main()
{
	cout << "**Starting Test**" << endl;

#ifdef VS
	ifstream  file("values-6.txt");
	if (!file.good())
	{
		cout << "**Unable to open input file?!" << endl;
		return -1;
	}
	istream&  input = file;
#else
	istream&   input = cin;
#endif

	//
	// first we input the size of the queue / problem:
	//
	int     N;
	input >> N;

	PQueue  pq(N);
	int     vertex;
	int     distance;

	//
	// now start executing commands:
	//
	string  cmd;
	input >> cmd;

	while (cmd != "exit")
	{
		if (cmd == "push")
		{
			input >> vertex;
			input >> distance;

			pq.Push(vertex, distance);

			cout << "Push: (" << vertex << "," << distance << ")" << endl;
		}
		else if (cmd == "pop")
		{
			cout << "PopMin: ";

			try
			{
				int v = pq.PopMin();
				cout << "vertex " << v;
			}
			catch (logic_error& le)
			{
				cout << le.what();
			}
			catch (...)
			{
				cout << "unknown exception...";
			}

			cout << endl;
		}
		else if (cmd == "empty")
		{
			cout << "Empty: " << pq.Empty() << endl;
		}
		else if (cmd == "dump")
		{
			string step;
			input >> step;
			pq.Dump("Step " + step);
		}
		else if (cmd == "fill")
		{
			int distance;
			input >> distance;

			cout << ">> filling..." << endl;

			pq.Fill(distance);
			pq.Dump("Filled:");
		}
		else if (cmd == "stress")
		{
			int result;
			int version;
			input >> version;

			cout << ">> stressing..." << endl;

			if (version == 1)
				result = StressTest1(pq, N);
			else if (version == 2)
				result = StressTest2(pq, N);
			else
			{
				cout << "**Error: unknown stress test version (" << version << "), no test run" << endl;
				result = -1;
			}

			if (result > -1)
			{
				cout << ">>stress test #" << version << " was successful!";
				cout << " [stressed " << result << " vertices]" << endl;
			}
			else
				cout << ">>stress test #" << version << " was *not* successful :-(" << endl;
		}
		else
		{
			cout << "**invalid cmd..." << endl;
		}

		input >> cmd;
	}

	cout << "**Done**" << endl;
	return 0;
}
