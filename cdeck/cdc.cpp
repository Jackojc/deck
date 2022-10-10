#include <utility>
#include <memory>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <string>
#include <string_view>
#include <iostream>
#include <iterator>

#include <cstddef>
#include <cstdint>

#include "cdc.hpp"

using namespace cdc;

int main(int argc, const char* argv[]) {
	std::ios_base::sync_with_stdio(false);
	std::cin.tie(nullptr);

	std::noskipws(std::cin);
	std::istream_iterator<char> it { std::cin }, end;
	std::string src { it, end };

	try {
		View sv { src.data(), src.data() + src.size() };

		Lexer lx { sv };
		Symbol sym = take(lx);  // Prepare the lexer.

		std::vector<Symbol> instructions = program(lx);

		discovery(instructions);
		// pretty_print(ctx);
	}

	catch (Report x) {
		report_handler(std::cerr, x);
	}

	return 0;
}
