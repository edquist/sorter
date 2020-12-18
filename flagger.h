#pragma once

#include <utility>


// initialize Op with -flag
template <class Op, class Flag>
struct flagger { Op operator-(Flag flag) const { return {flag}; } };


// initialize OpT<Flag> with -flag
template <template <class> class OpT>
struct flagger_t {
	template <class Flag>
	OpT<Flag> operator-(Flag &&flag) const
	{ return {std::forward<Flag>(flag)}; }
};

