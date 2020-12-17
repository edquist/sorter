#include <utility>
#include <algorithm>
#include <iostream>
#include <vector>
#include <initializer_list>


template <class T, class U>
std::ostream &operator<<(std::ostream &os, const std::pair<T,U> &p)
{ return os << p.first << '\t' << p.second; }


template <class Seq, class Op>
auto operator|(Seq &&seq, Op &&op) ->
decltype(op(seq.begin(), seq.end()))
{ return op(seq.begin(), seq.end()); }


// try to get the value_type for an iterator
template <class It>
using it_val_t = typename std::remove_const<
	typename std::remove_reference< 
		decltype(*std::declval<It>())
	>::type
>::type;


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


int main()
{
	auto seq = {99, 3, 1, 3, 3, 7, 99};
	auto v = seq | sort | uniq_c | sort_n;
	for (auto &&x : v)
		std::cout << x << "\n";
}

