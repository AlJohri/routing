#ifndef _messages
#define _messages

#include <iostream>

#include "node.h"
#include "link.h"

#if defined(GENERIC)
struct RoutingMessage {
 public:
  ostream & Print(ostream &os) const;
};
#endif

#if defined(LINKSTATE)
struct RoutingMessage {

  unsigned age;
  unsigned src;
  unsigned dest;
  unsigned cost;
  unsigned latency;  

  RoutingMessage();
  RoutingMessage(unsigned a, unsigned s, unsigned d, unsigned l); // src becomes nexthop at receiving node  
  RoutingMessage(const RoutingMessage &rhs);
  RoutingMessage &operator=(const RoutingMessage &rhs);

  unsigned GetSrc() const;
  unsigned GetDest() const;
  unsigned GetLatency() const;  

  ostream & Print(ostream &os) const;
};
#endif

#if defined(DISTANCEVECTOR)
struct RoutingMessage {

  unsigned src;
  unsigned dest;
  unsigned latency;


  RoutingMessage();
  RoutingMessage(unsigned s, unsigned d, unsigned l); // src becomes nexthop at receiving node
  RoutingMessage(const RoutingMessage &rhs);
  RoutingMessage &operator=(const RoutingMessage &rhs);

  unsigned GetSrc() const;
  unsigned GetDest() const;
  unsigned GetLatency() const;

  ostream & Print(ostream &os) const;
};
#endif


inline ostream & operator<<(ostream &os, const RoutingMessage &m) { return m.Print(os);}

#endif
