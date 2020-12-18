#pragma once

#include <utility>

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


#include "flagger.h"

constexpr
flagger_t<single_range>  echo;  // return range for single -value

