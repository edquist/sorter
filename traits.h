#pragma once

#include <utility>
#include <type_traits>


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

