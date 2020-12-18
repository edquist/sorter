#pragma once

#include "traits.h"
#include <utility>
#include <vector>
#include <stddef.h>

// range op: return range of unique items
struct uniq_lister {
	template <class It>
	using vtype = std::vector<it_val_t<It>>;

	template <class It, class End=It>
	auto operator()(It begin, End end) const -> vtype<It>
	{
		if (!(begin != end))  // if (begin == end)
			return {};

		vtype<It> v;
		it_val_t<It> val = *begin;
		for (It it = ++begin; it != end; ++it) {
			it_val_t<It> next = *it;
			if (next != val) {
				// c++20: co_yield {val};
				v.emplace_back(std::move(val));
				val = std::move(next);
			}
		}
		v.emplace_back(std::move(val));
		return v;
	}
} uniq;


// range op: return 'uniq -c'-like range of (count,item) pairs
struct uniq_counter {
	template <class It>
	using vtype = std::vector<std::pair<size_t, it_val_t<It>>>;

	template <class It, class End=It>
	auto operator()(It begin, End end) const -> vtype<It>
	{
		if (!(begin != end))  // if (begin == end)
			return {};

		vtype<It> v;
		it_val_t<It> val = *begin;
		size_t n = 1;
		for (It it = ++begin; it != end; ++it) {
			it_val_t<It> next = *it;
			if (next == val) {
				++n;
			} else {
				// c++20: co_yield {n, val};
				v.emplace_back(n, std::move(val));
				val = std::move(next);
				n = 1;
			}
		}
		v.emplace_back(n, std::move(val));
		return v;
	}
} uniq_c;



#include "optics.h"

// 'uniq -c' -> uniq_c
constexpr
uniq_counter operator-(uniq_lister, flag_option<'c'>) { return {}; }

