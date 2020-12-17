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


template <class It, class End=It>
struct iter_range {
	It  _begin, begin() const { return _begin; }
	End _end,   end()   const { return _end;   }
};


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


struct header {
	header_n operator-(size_t n) { return {n}; }
} head;


struct printer_os {
	std::ostream &os;

	template <class It, class End=It>
	void operator()(It begin, End end) const
	{
		for (It it = begin; it != end; ++it)
			std::cout << *it << "\n";
	}
};


struct printer {
	printer_os operator-(std::ostream &os) { return {os}; }
} printy;


int main()
{
	auto seq = {99, 3, 1, 3, 3, 7, 99,
	            4, 4, 4, 4,
	            2, 2, 2, 2, 2};

	seq | sort | uniq_c | sort_n | printy -std::cout;

	std::cout << "\n----------\n\n";

	seq | sort | uniq_c | sort_n | head -3 | printy -std::cout;
}

