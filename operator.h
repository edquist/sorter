#pragma once


// 'pipe' seq range into range op
template <class Seq, class Op>
auto operator|(Seq &&seq, Op &&op) ->
decltype(op(seq.begin(), seq.end()))
{ return op(seq.begin(), seq.end()); }

