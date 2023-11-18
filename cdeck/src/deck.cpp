#include <utility>
#include <iterator>

#include <string>
#include <string_view>

#include <sstream>
#include <iostream>

#include <cstddef>
#include <cstdint>

#include <deck/deck.hpp>

#include <deck/passes/dumper.hpp>
#include <deck/passes/printer.hpp>
#include <deck/passes/x86-64.hpp>

using namespace deck;

int main(int, const char*[]) {
	std::ios_base::sync_with_stdio(false);
	std::cin.tie(nullptr);

	std::noskipws(std::cin);
	std::istream_iterator<char> it { std::cin }, end;

	std::string src { it, end };

	try {
		Tree tree;

		tree = parse(std::move(src));

		tree = passes::dumper(std::move(tree));
		tree = passes::printer(std::move(tree));
		tree = passes::x86_64(std::move(tree));
	}

	catch (const Exception& e) {
		println(std::cerr, e.what());
	}

	return 0;
}
