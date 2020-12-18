#pragma once

#include <iosfwd>
#include <utility>


// range op: output items in range to ostream
struct printer_os {
	std::ostream &os;

	template <class It, class End=It>
	void operator()(It begin, End end) const
	{
		for (It it = begin; it != end; ++it)
			os << *it << "\n";
	}
};


// 'output' range to ostream
template <class Seq>
auto operator>(Seq &&seq, std::ostream &os) ->
decltype(std::forward<Seq>(seq) | printer_os{os} )
{ return std::forward<Seq>(seq) | printer_os{os}; }


#include "flagger.h"

constexpr
flagger<printer_os, std::ostream &>  printy;

