#pragma once

#include <iosfwd>
#include <utility>


template <class T, class U>
std::ostream &operator<<(std::ostream &os, const std::pair<T,U> &p)
{ return os << p.first << '\t' << p.second; }

