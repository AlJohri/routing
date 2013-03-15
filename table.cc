#include "table.h"
#include "messages.h"

#if defined(GENERIC)

ostream & Table::Print(ostream &os) const { os << "Table()"; return os; }

#endif

///////////////////////////////////////////////////////////////////////////////////////////////////////////

#if defined(LINKSTATE)

/* Adds outgoing links into the routing table for a node */
// Table::Table(Node me) {
//   deque<Link*> *links = me.GetOutgoingLinks();
//   for (deque<Link*>::iterator i = links->begin(); i != links->end(); ++i)


//      this->WriteTable((*i)->GetDest(), (*i)->GetDest(), (*i)->GetLatency());
//   delete links;
// }

int Table::updateLink(const Link *l) {
  //int currentAge = m[l->GetSrc()][l->GetDest()].age;  m[l->GetSrc()][l->GetDest()].age = ++currentAge;
  m[l->GetSrc()][l->GetDest()].cost = l->GetLatency();
  m[l->GetSrc()][l->GetDest()].distance = l->GetLatency();
  return m[l->GetSrc()][l->GetDest()].age++;
}

bool Table::updateTable(const RoutingMessage * message){
  if(m[message->src][message->dest].cost == -1) {
    m[message->src][message->dest].cost = message->latency; // message->cost ?
    m[message->src][message->dest].age = message->age;
    return true;
  }
  if(m[message->src][message->dest].age < (int) message->age) {
    m[message->src][message->dest].cost = message->latency; // message->cost ?
    m[message->src][message->dest].age = message->age;
    return true;
  }
  return false;
}

void Table::printTable()
{
  cout << "PRINT TABLE" << endl;
   for(map<int, map<int,TopoLink> >::const_iterator itr = m.begin(); itr != m.end(); ++itr) {
      cout << (itr)->first << ": "; // source
       for(map<int, TopoLink >::const_iterator itr2 = (itr)->second.begin(); itr2 != (itr)->second.end(); ++itr2) {
         cout << (itr2)->first << "(" << (itr2)->second.cost << "), "; // destination cost
       }
       cout << endl;
    }
  cout<< "----------------------------" << endl;  
}


#endif

///////////////////////////////////////////////////////////////////////////////////////////////////////////

#if defined(DISTANCEVECTOR)

Table::Table() {}

/* Adds outgoing links into the routing table for a node */
Table::Table(Node me) {
  deque<Link*> *links = me.GetOutgoingLinks();
  for (deque<Link*>::iterator i = links->begin(); i != links->end(); ++i)
     this->WriteTable((*i)->GetDest(), (*i)->GetDest(), (*i)->GetLatency());
  delete links;
}

/* Checks if latency is less than the latency in the routng table */
bool Table::CheckLatency(unsigned dest, unsigned nxt, double dist) {
  //if (table.size() <= dest || table[dest].size() <= nxt) return 0;
  if (this->ReadTable(dest, nxt) > dist || this->ReadTable(dest, nxt) == -1) return 1;
  return 0;
}

/* Updates latency regardless of its value */
bool Table::WriteTable(unsigned dest, unsigned nxt, double dist) {
  while (table.size() <= dest) table.push_back(*new(vector<double>));
  while (table[dest].size() <= nxt) table[dest].push_back((double) -1);
  unsigned ret = CheckLatency(dest, nxt, dist);
  table[dest][nxt] = dist;
  return ret;
}

double Table::ReadTable(unsigned dest, unsigned nxt) {
  if (table.size() <= dest || table[dest].size() <= nxt) return -1;
  return table[dest][nxt];
}

unsigned Table::GetNext(unsigned dest) {
  if (table.size() <= dest) {
    //cerr << "table size: " << table.size() << " dest: " << dest << endl;
    return -1;
  }

  unsigned ret = 0;
  unsigned counter = 0;
  for (vector<double>::const_iterator i = table[dest].begin(); i != table[dest].end(); ++i) {
      if ((*i) != -1 && ((*i) < table[dest][ret] || table[dest][ret] == -1))
          ret = counter;

      counter++;
  }
  return ret;
}

ostream & Table::Print(ostream &os) const {
  if (this == NULL) return os;
  os << endl << "Table: " << endl;
  int counter = 0;
  for (vector<vector<double> >::const_iterator i = table.begin(); i != table.end(); ++i) {
      os << "to " << counter++ << ": ";
      for (vector<double>::const_iterator j = (*i).begin(); j != (*i).end(); ++j)
          os << "\t" << (*j);
      os << endl;
  }
  return os;
}

#endif