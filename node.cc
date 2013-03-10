#include "node.h"
#include "context.h"
#include "error.h"


Node::Node(const unsigned n, SimulationContext *c, double b, double l) : 
    number(n), context(c), bw(b), lat(l) 
{
    if (c != NULL){
        this->rt = new Table(*this);

    cerr << "now: " << endl << *this;}
}

Node::Node() 
{ throw GeneralException(); }

Node::Node(const Node &rhs) : 
  number(rhs.number), context(rhs.context), bw(rhs.bw), lat(rhs.lat), rt(rhs.rt) {}

Node & Node::operator=(const Node &rhs) 
{
  return *(new(this)Node(rhs));
}

void Node::SetNumber(const unsigned n) 
{ number=n;}

unsigned Node::GetNumber() const 
{ return number;}

void Node::SetLatency(const double l)
{ lat=l;}

double Node::GetLatency() const 
{ return lat;}

void Node::SetBW(const double b)
{ bw=b;}

double Node::GetBW() const 
{ return bw;}

Node::~Node()
{}

// Implement these functions  to post an event to the event queue in the event simulator
// so that the corresponding node can recieve the ROUTING_MESSAGE_ARRIVAL event at the proper time
void Node::SendToNeighbors(const RoutingMessage *m)
{
}

void Node::SendToNeighbor(const Node *n, const RoutingMessage *m)
{

}

deque<Node*> *Node::GetNeighbors()
{
  return context->GetNeighbors(this);
}

deque<Link*> *Node::GetOutgoingLinks()
{
    return context->GetOutgoingLinks(this);
}

void Node::SetTimeOut(const double timefromnow)
{
  context->TimeOut(this,timefromnow);
}


bool Node::Matches(const Node &rhs) const
{
  return number==rhs.number;
}


#if defined(GENERIC)
void Node::LinkHasBeenUpdated(const Link *l)
{
  cerr << *this << " got a link update: "<<*l<<endl;
  //Do Something generic:
  SendToNeighbors(new RoutingMessage);
}


void Node::ProcessIncomingRoutingMessage(const RoutingMessage *m)
{
  cerr << *this << " got a routing messagee: "<<*m<<" Ignored "<<endl;
}


void Node::TimeOut()
{
  cerr << *this << " got a timeout: ignored"<<endl;
}

Node *Node::GetNextHop(const Node *destination)
{
  return 0;
}

Table *Node::GetRoutingTable() const
{
  return new Table;
}


ostream & Node::Print(ostream &os) const
{
  os << "Node(number="<<number<<", lat="<<lat<<", bw="<<bw<<")";
  return os;
}

#endif

#if defined(LINKSTATE)


void Node::LinkHasBeenUpdated(const Link *l)
{
  cerr << *this<<": Link Update: "<<*l<<endl;
}


void Node::ProcessIncomingRoutingMessage(const RoutingMessage *m)
{
  cerr << *this << " Routing Message: "<<*m;
}

void Node::TimeOut()
{
  cerr << *this << " got a timeout: ignored"<<endl;
}

Node *Node::GetNextHop(const Node *destination)
{
  // WRITE
  return 0;
}

Table *Node::GetRoutingTable() const
{
  // WRITE
  return 0;
}


ostream & Node::Print(ostream &os) const
{
  os << "Node(number="<<number<<", lat="<<lat<<", bw="<<bw<<")";
  return os;
}
#endif


#if defined(DISTANCEVECTOR)

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
  // update our table
  // send out routing mesages
  cerr << "Should I update my routing table with this? : " << endl;
  cerr << *this<<": Link Update: "<<*l<<endl;
  // WriteTable(); src->dest latency to me->dest latency
  // is src always 1 hop from me? 
  // then first hop is always src
 

    if (this->rt->WriteTable(l->GetDest(), l->GetDest(), l->GetLatency())) {
        this->UpdateNeighbors(l->GetDest(), l->GetLatency());
    }
}


void Node::ProcessIncomingRoutingMessage(const RoutingMessage *m)
{
    cerr << "PROCESS" << endl << *m << endl;

    Link *l = context->FindMatchingLink(new Link(this->GetNumber(), m->GetSrc(), 0, 0, 0));

    if (this->rt->WriteTable(m->GetDest(), m->GetSrc(), m->GetLatency() + l->GetLatency())) {
        this->UpdateNeighbors(m->GetDest(), m->GetLatency() + l->GetLatency());
    }

}

void Node::TimeOut()
{
  cerr << *this << " got a timeout: ignored"<<endl;
}


Node *Node::GetNextHop(const Node *destination)
{

    //cerr << *this << " next hop: " << *destination << endl;

    deque<Node*> *nodes = this->GetNeighbors();

    unsigned ret = this->rt->GetNext(destination->GetNumber());

    //cerr << *this;

    for (deque<Node*>::const_iterator i = nodes->begin(); i != nodes->end(); ++i) {
        if ((Node(ret, 0, 0, 0).Matches(**i))) {
            cerr << "Success: " << ret << ": " << destination->GetNumber() << endl;
            return new Node(**i); // change to new node **i
        }
    }

    return 0;
}

Table *Node::GetRoutingTable() const
{
    return new Table(*(this->rt)); // change to new Table
    //return 0;
}


ostream & Node::Print(ostream &os) const
{
  os << "Node(number="<<number<<", lat="<<lat<<", bw="<<bw<<")" << *rt;
  return os;
}
#endif
