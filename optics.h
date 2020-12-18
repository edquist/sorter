#pragma once


// valueless type-constants representing options for converting types
template <char Flag>
struct flag_option {};

namespace optics {
constexpr flag_option<'n'> n;
constexpr flag_option<'c'> c;
}

