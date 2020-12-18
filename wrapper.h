#pragma once


// range to wrap an arbitrary iterator pair
template <class It, class End=It>
struct iter_range {
	It  _begin, begin() const { return _begin; }
	End _end,   end()   const { return _end;   }
};

