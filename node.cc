#include "node.h"
#include "context.h"
#include "error.h"
#include <vector>
#include <set>
#include <map>

#define MAX 2147483647

Node::Node(const unsigned n, SimulationContext *c, double b, double l) : number(n), context(c), bw(b), lat(l) 
{
  
  #if defined(DISTANCEVECTOR)
    if (c != NULL) {
      this->rt = new Table(*this);
      //cerr << "now: " << endl << *this;
    }
  #endif

  #if defined(LINKSTATE)
    if (c != NULL) {
      this->topo = new Table(); //(*this);
      //cerr << "now: " << endl << *this;
    }
  #endif    
}

Node::Node() { throw GeneralException(); }
Node::~Node() {}
Node & Node::operator=(const Node &rhs) { return *(new(this)Node(rhs)); }
void Node::SetNumber(const unsigned n) { number=n; }
unsigned Node::GetNumber() const { return number; }
void Node::SetLatency(const double l) { lat=l; }
double Node::GetLatency() const  { return lat; }
void Node::SetBW(const double b) { bw=b; }
double Node::GetBW() const { return bw; }

deque<Node*> *Node::GetNeighbors() { return context->GetNeighbors(this); }
deque<Link*> *Node::GetOutgoingLinks() { return context->GetOutgoingLinks(this); }

void Node::SendToNeighbors(const RoutingMessage *m)
{

  deque<Node*> * neighbors = GetNeighbors();
  deque<Node*>::iterator itr;
  for(itr = neighbors->begin(); itr != neighbors->end(); ++itr){
    const RoutingMessage * l = new RoutingMessage(*m);
    SendToNeighbor(*itr,l);
  }

}

void Node::SendToNeighbor(const Node *n, const RoutingMessage *m)
{
  Link * temp = new Link();
  temp->SetSrc(this->number);
  temp->SetDest(n->number);

  const Link * l = new Link(*temp);
  Link * real = context->FindMatchingLink(l);
  if(real != 0) {
   Event * event = new Event(context->GetTime()+real->GetLatency(),ROUTING_MESSAGE_ARRIVAL, new Node(*n), new RoutingMessage(*m));

   context->PostEvent(event);
  }

  delete l;
  delete temp;
}

void Node::SetTimeOut(const double timefromnow) { context->TimeOut(this,timefromnow); }
bool Node::Matches(const Node &rhs) const { return number==rhs.number; }

///////////////////////////////////////////////////////////////////////////////////////////////////////////

#if defined(GENERIC)

void Node::LinkHasBeenUpdated(const Link *l) { cerr << *this << " got a link update: "<<*l<<endl; SendToNeighbors(new RoutingMessage); /* something generic */ }
void Node::ProcessIncomingRoutingMessage(const RoutingMessage *m) { cerr << *this << " got a routing messagee: " << *m << " Ignored "<<endl; }
void Node::TimeOut() { cerr << *this << " got a timeout: ignored"<<endl; }
Node *Node::GetNextHop(const Node *destination) { return 0; }
Table *Node::GetRoutingTable() const { return new Table; }
ostream & Node::Print(ostream &os) const { os << "Node(number="<<number<<", lat="<<lat<<", bw="<<bw<<")"; return os; }

#endif

///////////////////////////////////////////////////////////////////////////////////////////////////////////

#if defined(LINKSTATE)

Node::Node(const Node &rhs) : number(rhs.number), context(rhs.context), bw(rhs.bw), lat(rhs.lat), topo(rhs.topo) {}

void Node::LinkHasBeenUpdated(const Link *l) { 
  int age = topo->updateLink(l);
  const RoutingMessage * message = new RoutingMessage(age, l->GetSrc(), l->GetDest(), l->GetLatency());
  SendToNeighbors(message);
}
void Node::ProcessIncomingRoutingMessage(const RoutingMessage *m) { 
  //topo->printTable();
  if(topo->updateTable(m)) {
    SendToNeighbors(m);
  }
}
void Node::TimeOut() { /*cerr << *this << " got a timeout: ignored" << endl;*/ }


Node *Node::GetNextHop(const Node *destination) {
  topo->printTable();
  if(number == destination->number)
	return this;
  
  unsigned numNodes = (topo->m).size();

  /*Djikstra's Algorithm*/
  vector<int>  distances(numNodes, MAX);        // <x, D(x)> where D(x) is the distnace from node n to destination
  vector<int>  parents(numNodes, MAX);          // <x, p(x)> where p(x) is the parent node to x in tree traversal
  vector<bool> checked(numNodes, false);   //

  unsigned workingSetDistance = 0;


  //INITIALIZE SHIT
  
  // iterate over all destinations from current node using topology table 
  // first iteration - neighbors of the original node
  checked[number] = true;
  distances[number] = 0;
  parents[number] = 99999;
  workingSetDistance = 0;

  for(map<int, TopoLink>::const_iterator i = topo->m[number].begin(); i != topo->m[number].end(); i++) {
    distances[i->first] = i->second.cost;
    parents[i->first] = number;
	
	/*if((i->first == destination->number) && (i->second.cost != MAX)) {
		return this;
	}*/
  }
  
  
 

  // LOOP DE LOOP
  for(int i = 0; i < numNodes - 1; i++) {
    cout << "loop1: " << i << "          " << "" << endl;
    // iterate over distances vector and find minimum distance
    int minNode;
    int minDistance = MAX;
    
    for(int i = 0; i < distances.size(); i++) {
     // cout << "     loop2: " << i << "     " << minNode << " " << minDistance << endl;

      if (checked[i] == false) {
        if(minDistance > distances[i]) {
          minNode = i;
          minDistance = distances[i];
        }
      }

    }

    cout << minNode << " " << minDistance << endl;
    cout << "Nodes: " << " 0  1  2  3  4" << endl;
    cout << "Distances Vector: " << distances[0] << " " << distances[1] << " " << distances[2] << " " << distances[3] << " " << distances[4] << endl;
    cout << "Parents Vector: " << parents[0] << " " << parents[1] << " " << parents[2] << " " << parents[3] << " " << parents[4] << endl;
    cout << endl;

    // insert minNode found into the working set and set checked to true
    checked[minNode] = true;
	  if(minDistance != MAX)
	    workingSetDistance = minDistance;
		
	cout << "workingSetDistance: " << workingSetDistance << endl;
    // construct djikstra table using the minNode
    for(map<int, TopoLink>::const_iterator i = topo->m[minNode].begin(); i != topo->m[minNode].end(); ++i) {
  	  if (distances[i->first] > (i->second.cost + workingSetDistance) && checked[i->first] == false) {
  	    distances[i->first] = i->second.cost + workingSetDistance;
  		parents[i->first] = minNode;
      }
    }
  }

  cout << endl;
  cout << endl;
  cout << "Initial node is : " << this->number <<endl;
  cout << "destination is " << destination->number << endl;
  cout << "Parent of destination is " << parents[destination->number] << endl;
  
  //construct routing table
  int currentNode = destination->number;
  int currentParent = parents[destination->number];

	
  while (currentParent!=number) {	  
	  currentNode = currentParent;
	  currentParent = parents[currentNode];
  } 
  
  //cout << "Route this packet to " << currentNode << endl;
  
  deque<Node*> *nodes = this->GetNeighbors();
  for (deque<Node*>::const_iterator i = nodes->begin(); i != nodes->end(); ++i) {
      if ((Node(currentNode, 0, 0, 0).Matches(**i))) {
          //cerr << "Success: " << ret << ": " << destination->GetNumber() << endl;
          return new Node(**i); // change to new node **i
      }
  }  

}
Table *Node::GetRoutingTable() const { /* WRITE */ return 0; }
ostream & Node::Print(ostream &os) const { os << "Node(number=" << number << ", lat=" << lat << ", bw=" << bw << ")"; return os; }

#endif

///////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

#if defined(DISTANCEVECTOR)

Node::Node(const Node &rhs) : number(rhs.number), context(rhs.context), bw(rhs.bw), lat(rhs.lat), rt(rhs.rt) {}

void Node::UpdateNeighbors(unsigned dest, unsigned latency) {
    deque<Link*> *links = this->GetOutgoingLinks();
    deque<Node*> *nodes = this->GetNeighbors();
    RoutingMessage *message = new RoutingMessage(this->GetNumber(), dest, latency);

    Node *curr;
    for (deque<Link*>::iterator i = links->begin(); i != links->end(); ++i) {
      for (deque<Node*>::iterator j = nodes->begin(); j != nodes->end(); ++j) {
        if (Node((*i)->GetDest(), 0, 0, 0).Matches(**j)) {
          curr = *j;
          break;
        }
      }
      Event *event = new Event(context->GetTime() + (*i)->GetLatency(), ROUTING_MESSAGE_ARRIVAL, curr, message);
      context->PostEvent(event);
    }
    delete links;
    delete nodes;
}

void Node::LinkHasBeenUpdated(const Link *l)
{
  //cerr << *this << ": Link Update: " << *l << endl;

  // write regardless of latency
    this->rt->WriteTable(l->GetDest(), l->GetDest(), l->GetLatency());
    //cerr << "Just updated: " << l->GetLatency() << endl << *this << endl;
    this->UpdateNeighbors(l->GetDest(), l->GetLatency());
}

void Node::ProcessIncomingRoutingMessage(const RoutingMessage *m)
{
    // write only when latency is less
    //cerr << "PROCESS" << endl << *m << endl;
    Link *l = context->FindMatchingLink(new Link(this->GetNumber(), m->GetSrc(), 0, 0, 0));

    if (this->rt->CheckLatency(m->GetDest(), m->GetSrc(), m->GetLatency() + l->GetLatency())) {
        this->rt->WriteTable(m->GetDest(), m->GetSrc(), m->GetLatency() + l->GetLatency());
        this->UpdateNeighbors(m->GetDest(), m->GetLatency() + l->GetLatency());
        //cerr << "BOOM: " << endl << *this << endl;
    }

    // if (this->rt->WriteTable3(m->GetDest(), m->GetSrc(), m->GetLatency() + l->GetLatency()))
    //   this->UpdateNeighbors(m->GetDest(), m->GetLatency() + l->GetLatency());

}

void Node::TimeOut() { cerr << *this << " got a timeout: ignored" << endl; }

Node *Node::GetNextHop(const Node *destination)
{

  //cerr << *this << " next hop: " << *destination << endl;
  deque<Node*> *nodes = this->GetNeighbors();
  unsigned ret = this->rt->GetNext(destination->GetNumber());
  for (deque<Node*>::const_iterator i = nodes->begin(); i != nodes->end(); ++i) {
      if ((Node(ret, 0, 0, 0).Matches(**i))) {
          //cerr << "Success: " << ret << ": " << destination->GetNumber() << endl;
          return new Node(**i); // change to new node **i
      }
  }

  return 0;
}

// change to new Table
Table *Node::GetRoutingTable() const { return new Table(*(this->rt));  }
ostream & Node::Print(ostream &os) const { os << "Node(number="<<number<<", lat="<<lat<<", bw="<<bw<<")" << *rt; return os; }

#endif