// misc range-like helpers to facilitate composing shell-like pipelines

#include "broker.h"
#include "echo.h"
#include "header.h"
#include "mapper.h"
#include "operator.h"
#include "optics.h"
#include "pairstacker.h"
#include "printer.h"
#include "sorter.h"
#include "streamer.h"
#include "uniq.h"

#include <iostream>
#include <initializer_list>


int main()
{
	using optics::c;
	using optics::n;

	auto seq = {99, 3, 1, 3, 3, 7, 99,
	            4, 4, 4, 4,
	            2, 2, 2, 2, 2};

	seq | sort | uniq                               > std::cout;

	echo -"\n----------\n"                          > std::cout;

	seq | sort | uniq -c | sort -n                  > std::cout;

	echo -"\n----------\n"                          > std::cout;

	seq | sort | uniq -c | sort -n | head -3        > std::cout;

	echo -"\n----------\n"                          > std::cout;

	seq | sort | uniq -c | sort -n | map -pairstack > std::cout;
}

