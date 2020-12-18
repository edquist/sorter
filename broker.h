#pragma once

#include "traits.h"
#include <iosfwd>
#include <vector>  // *shrug*

// break-out broker for reversing operator precedence
template <class Op>
struct broker {
	Op op;
	std::ostream &os;
};


// let broker handle 'op > out' if op takes a range (rather than returns one)
template <class Op>
inline
auto operator>(Op &&op, std::ostream &os)
  -> first_t<broker<Op>, decltype(op(std::declval<std::vector<int>>().begin(),
                                     std::declval<std::vector<int>>().end()))>
{ return {std::forward<Op>(op), os}; }


// break-out items from broker to reverse '|' vs '>' operator precedence
// seq | broker {op, out} -> (seq | op) > out
template <class Seq, class Broker>
inline
auto operator|(Seq &&seq, Broker &&b) ->
decltype((std::forward<Seq>(seq) | std::forward<Broker>(b).op)
                                 > std::forward<Broker>(b).os)
{ return (std::forward<Seq>(seq) | std::forward<Broker>(b).op)
                                 > std::forward<Broker>(b).os; }

