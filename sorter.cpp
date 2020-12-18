// misc range-like helpers to facilitate composing shell-like pipelines


#include <utility>
#include <algorithm>
#include <iostream>
#include <vector>
#include <initializer_list>


template <class T, class U>
std::ostream &operator<<(std::ostream &os, const std::pair<T,U> &p)
{ return os << p.first << '\t' << p.second; }


// 'pipe' seq range into range op
template <class Seq, class Op>
auto operator|(Seq &&seq, Op &&op) ->
decltype(op(seq.begin(), seq.end()))
{ return op(seq.begin(), seq.end()); }


// helper template to get the value_type for an iterator
template <class It>
using it_val_t = typename std::remove_const<
	typename std::remove_reference< 
		decltype(*std::declval<It>())
	>::type
>::type;


// return first type; discard other types used to test expressions
template <class T, class ...>
using first_t = T;


// range to wrap an arbitrary iterator pair
template <class It, class End=It>
struct iter_range {
	It  _begin, begin() const { return _begin; }
	End _end,   end()   const { return _end;   }
};


// range to wrap a single item
template <class T>
struct single_range {
	T item;

	struct end_iterator {};

	struct iterator {
		T item;
		bool done;

		iterator &operator++() { done = true; return *this; }
		T operator*() const { return item; }

		bool operator!=(end_iterator) const { return !done; }
	};

	iterator     begin() const { return {std::forward<T>(item)}; }
	end_iterator   end() const { return {}; }
};


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



// initialize Op with -flag
template <class Op, class Flag>
struct flagger { Op operator-(Flag flag) { return {flag}; } };

flagger<header_n,   size_t>          head;
flagger<printer_os, std::ostream &>  printy;


// initialize OpT<Flag> with -flag
template <template <class> class OpT>
struct flagger_t {
	template <class Flag>
	OpT<Flag> operator-(Flag &&flag) { return {std::forward<Flag>(flag)}; }
};

flagger_t<single_range> echo;  // return range for single -value
flagger_t<mapper_op>    map;   // return range transformed with -operator


// valueless type-constants representing options for converting types
template <char Flag>
struct flag_option {};

flag_option<'n'> n;
flag_option<'c'> c;


// 'sort -n' is the same as sort
sorter operator-(sorter, flag_option<'n'>) { return {}; }

// 'uniq -c' -> uniq_c
uniq_counter operator-(uniq_lister, flag_option<'c'>) { return {}; }


// 'output' range to ostream
template <class Seq>
auto operator>(Seq &&seq, std::ostream &os) ->
decltype(std::forward<Seq>(seq) | printer_os{os} )
{ return std::forward<Seq>(seq) | printer_os{os}; }


template <class Op>
struct broker {
	Op op;
	std::ostream &os;
};


template <class Op>
auto operator>(Op &&op, std::ostream &os)
  -> first_t<broker<Op>, decltype(op(std::declval<std::vector<int>>().begin(),
                                     std::declval<std::vector<int>>().end()))>
{ return {std::forward<Op>(op), os}; }


template <class Seq, class Broker>
auto operator|(Seq &&seq, Broker &&b) ->
decltype((std::forward<Seq>(seq) | std::forward<Broker>(b).op)
                                 > std::forward<Broker>(b).os)
{ return (std::forward<Seq>(seq) | std::forward<Broker>(b).op)
                                 > std::forward<Broker>(b).os; }


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


int main()
{
	auto seq = {99, 3, 1, 3, 3, 7, 99,
	            4, 4, 4, 4,
	            2, 2, 2, 2, 2};

	seq | sort | uniq > std::cout;

	echo -"\n----------\n" | printy -std::cout;

	seq | sort | uniq -c | sort -n | printy -std::cout;

	echo -"\n----------\n" | printy -std::cout;

	seq | sort | uniq -c | sort -n | head -3 | printy -std::cout;

	echo -"\n----------\n" | printy -std::cout;

	seq | sort | uniq -c | sort -n | map -pairstack | printy -std::cout;
}

