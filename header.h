#pragma once

#include "wrapper.h"
#include <stddef.h>


// range op: return range over first n items
struct header_n {
	size_t n;

	template <class It>
	struct iterator {
		It it;
		size_t n;

		auto operator*() -> decltype(*it) { return *it; }
		iterator &operator++() { ++it; ++n; return *this; }

		template <class End>
		bool operator!=(const iterator<End> &end) const
		{ return n != end.n && it != end.it; }
	};

	template <class It, class End=It>
	auto operator()(It begin, End end) const
	  -> iter_range<iterator<It>, iterator<End>>
	{ return {{begin, 0}, {end, n}}; }
};


#include "flagger.h"

constexpr
flagger<header_n, size_t>  head;

