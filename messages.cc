#include "messages.h"

#if defined(GENERIC)
ostream &RoutingMessage::Print(ostream &os) const { os << "RoutingMessage()"; return os; }
#endif

#if defined(LINKSTATE)

ostream &RoutingMessage::Print(ostream &os) const { return os; }
RoutingMessage::RoutingMessage() {}
RoutingMessage::RoutingMessage(unsigned a, unsigned s, unsigned d, unsigned l): age(a), src(s), dest(d), latency(l) {}
RoutingMessage::RoutingMessage(const RoutingMessage &rhs): age(rhs.age), src(rhs.src), dest(rhs.dest), latency(rhs.latency) {}

unsigned RoutingMessage::GetSrc() const { return src; }
unsigned RoutingMessage::GetDest() const { return dest; }
unsigned RoutingMessage::GetLatency() const { return latency; }

#endif

#if defined(DISTANCEVECTOR)

ostream &RoutingMessage::Print(ostream &os) const
{ os << "New path from " << src << " to " << dest << " of " << latency << endl; return os; }

RoutingMessage::RoutingMessage() {}
RoutingMessage::RoutingMessage(unsigned s, unsigned d, unsigned l): src(s), dest(d), latency(l) {}
RoutingMessage::RoutingMessage(const RoutingMessage &rhs): src(rhs.src), dest(rhs.dest), latency(rhs.latency) {}

unsigned RoutingMessage::GetSrc() const { return src; }
unsigned RoutingMessage::GetDest() const { return dest; }
unsigned RoutingMessage::GetLatency() const { return latency; }

#endif

