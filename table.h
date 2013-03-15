#ifndef _table
#define _table

#include <iostream>
#include <vector>
#include <map>
#include "node.h"
#include "link.h"

//using namespace std;

#if defined(GENERIC)
class Table {
 public:
  ostream & Print(ostream &os) const;
};
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////////////

#if defined(LINKSTATE)

class TopoLink {
public:
  int cost;
  int age;
  int distance;
  TopoLink() { cost = -1; age = 0; distance = -1; };
  TopoLink(int c, int a) { cost = c; age = a; distance = -1; };
};

class Table {
public:
  map<int, map<int, TopoLink> > m; // toplogy table of links <source, destination>
  Table() {};
  bool updateTable(const RoutingMessage * message);
  int updateLink(const Link *l);
  void printTable();
  void find(int *src, int *dest);
  ostream & Print(ostream &os) const;
};

#endif

///////////////////////////////////////////////////////////////////////////////////////////////////////////

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
  bool CheckLatency(unsigned src, unsigned nxt, double dist);
  bool WriteTable(unsigned src, unsigned nxt, double dist);
  double ReadTable(unsigned src, unsigned nxt);
  unsigned GetNext(unsigned dest);
  ostream & Print(ostream &os) const;
};

#endif

inline ostream & operator<<(ostream &os, const Table &t) { return t.Print(os);}


#endif
