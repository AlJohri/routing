#ifndef _table
#define _table


#include <iostream>
#include <vector>
#include "node.h"
#include "link.h"

using namespace std;

#if defined(GENERIC)
class Table {
  // Students should write this class

 public:
  ostream & Print(ostream &os) const;
};
#endif


#if defined(LINKSTATE)
class Table {
  // Students should write this class
 public:
  ostream & Print(ostream &os) const;
};
#endif

#if defined(DISTANCEVECTOR)

#include <deque>

class Node;
class Link;

class Table {
private: 
    vector<vector<double> > table;

 public:

    Table();
    Table(Node me);
    int GetSize();
    bool WriteTable(unsigned src, unsigned nxt, double dist);
    double ReadTable(unsigned src, unsigned nxt);
    unsigned GetNext(unsigned dest);

  ostream & Print(ostream &os) const;
};
#endif

inline ostream & operator<<(ostream &os, const Table &t) { return t.Print(os);}

#endif
