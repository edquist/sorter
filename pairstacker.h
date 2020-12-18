#pragma once

#include <utility>


// map (n,m) -> n*1000 + m
struct pairstacker {
	template <class Pair>
	auto operator()(Pair &&p) ->
	decltype(std::forward<Pair>(p).first + std::forward<Pair>(p).second)
	{
		return std::forward<Pair>(p).first * 1000
		     + std::forward<Pair>(p).second;
	}
} pairstack;

