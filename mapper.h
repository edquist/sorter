#pragma once

#include "wrapper.h"


// range op: return range transformed with op
template <class Op>
struct mapper_op {
	Op op;

	template <class End>
	struct end_iterator { End it; };

	template <class It>
	struct iterator {
		It it;
		Op op;

		auto operator*() -> decltype(op(*it)) { return op(*it); }
		iterator &operator++() { ++it; return *this; }

		template <class End>
		bool operator!=(const End &end) const
		{ return it != end.it; }
	};

	template <class It, class End=It>
	auto operator()(It begin, End end) const
	  -> iter_range<iterator<It>, end_iterator<End>>
	{ return {{begin, op}, {end}}; }
};


#include "flagger.h"

constexpr
flagger_t<mapper_op>  map;   // return range transformed with -operator

