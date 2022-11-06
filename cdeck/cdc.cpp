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

#include "passes/quotes.hpp"
#include "passes/discover.hpp"
#include "passes/verify.hpp"
#include "passes/pretty_print.hpp"
#include "passes/refgraph.hpp"
#include "passes/ranges.hpp"
#include "passes/reorder.hpp"
#include "passes/emit.hpp"

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

		quotes(ctx, instructions);
		discover(ctx, instructions);
		verify(ctx, instructions);
		refgraph(ctx, instructions);
		ranges(ctx, instructions);
		reorder(ctx, instructions);
		emit(ctx, instructions);
		pretty_print(ctx, instructions);

		// for (auto it = instructions.begin(); it != instructions.end(); ++it)
		// 	std::cout << *it << '\n';
	}

	catch (Report x) {
		report_handler(std::cerr, x);
	}

	return 0;
}
