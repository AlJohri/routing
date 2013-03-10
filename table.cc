#include "table.h"

#if defined(GENERIC)
ostream & Table::Print(ostream &os) const
{
  // WRITE THIS
  os << "Table()";
  return os;
}
#endif

#if defined(LINKSTATE)

#endif

#if defined(DISTANCEVECTOR)

Table::Table() {cerr << "FUCK!" << endl;}

//Table::Table(const Table &rhs) {

    // iterate through, push_back (new) vectors and doubles

//}

Table::Table(Node me) {
    // initial state: check my outgoing links, WriteTable(), send initial state to neighbors

     


    deque<Link*> *links = me.GetOutgoingLinks();

    for (deque<Link*>::iterator i = links->begin(); i != links->end(); ++i) {

       this->WriteTable((*i)->GetDest(), (*i)->GetDest(), (*i)->GetLatency());

    }

   delete links;

}

//int Table::GetSize() {
//    return table.GetSize();
//}

bool Table::WriteTable(unsigned dest, unsigned nxt, double dist) {
    cerr << "WRITE" << endl;
    int start = table.size();
    while (table.size() <= dest) {
        // table too short

        vector<double> *newrow = new(vector<double>);

        table.push_back(*newrow);

    }

    while (table[dest].size() <= nxt) {

        table[dest].push_back((double) -1);

    }

    if (table[dest][nxt] > dist || table[dest][nxt] == -1) {
        table[dest][nxt] = dist;
        return dist;
    }

    return 0;
}

double Table::ReadTable(unsigned dest, unsigned nxt) {
    cerr << "READ" << endl;
    if (table.size() < dest || table[dest].size() < nxt)
        return -1;

    return table[dest][nxt];
}

unsigned Table::GetNext(unsigned dest) { // BROKEN
    //cerr << "FUCK" << endl;
    //cerr << "GET NEXT" << endl << "Width: " << table.size() << endl << "Height: " << table[dest].size() << endl << dest << endl;
    
    //cerr << *this << endl;
    
    if (table.size() <= dest) {
        cerr << "table size: " << table.size() << " dest: " << dest << endl;
        return -1;
    }
    unsigned ret = 0;
    unsigned counter = 0;
    for (vector<double>::const_iterator i = table[dest].begin(); i != table[dest].end(); ++i) {
        //cerr << (*i) << endl;
        if ((*i) != -1 && ((*i) < table[dest][ret] || table[dest][ret] == -1)) // RIGHT HERE
            ret = counter;
        counter++;
    }
    return ret;
}

ostream & Table::Print(ostream &os) const {

    if (this == NULL)
        return os;

    os << endl;
    os << "Table: " << endl;

    vector<vector<double> >::const_iterator i = table.begin();
    int counter = 0;

    for (; i != table.end(); ++i) {

        os << "to "<< counter++ << ": ";

        for (vector<double>::const_iterator j = (*i).begin(); j != (*i).end(); ++j) {
            
            os << "\t" << (*j);

        }

        os << endl;

    }
    cerr << "PRINT OUT" << endl;

    return os;
}


#endif
