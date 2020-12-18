#pragma once

#include "traits.h"

#include <algorithm>
#include <vector>


// range op: return sorted range
struct sorter {
	template <class It>
	using vtype = std::vector<it_val_t<It>>;

	template <class It, class End=It>
	auto operator()(It begin, End end) const -> vtype<It>
	{
		vtype<It> v {begin, end};
		std::sort(v.begin(), v.end());
		return v;
	}
} sort, sort_n;


#include "optics.h"

// 'sort -n' is the same as sort
constexpr
sorter operator-(sorter, flag_option<'n'>) { return {}; }

