#include <utility>
#include <algorithm>
#include <memory>
#include <vector>
#include <list>
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

		Context ctx;
		std::vector<Symbol> instructions = program(lx);

		reduce_quotes(ctx, instructions.begin(), instructions);
		discover(ctx, instructions.begin(), instructions);
		verify(ctx, instructions.begin(), instructions);
		pretty_print(ctx, instructions.begin(), instructions);
		emit(ctx, instructions.begin(), instructions);

		// for (auto it = instructions.begin(); it != instructions.end(); ++it)
		// 	std::cout << *it << '\n';
	}

	catch (Report x) {
		report_handler(std::cerr, x);
	}

	return 0;
}
