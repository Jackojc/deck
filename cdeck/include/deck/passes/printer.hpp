#ifndef DECK_PASS_PRINTER_HPP
#define DECK_PASS_PRINTER_HPP

/*
	This pass pretty-prints the AST with a nested structure.
*/
#include <cstddef>
#include <cstdint>

#include <utility>
#include <iostream>

#include <array>

#include <deck/deck.hpp>

namespace deck::passes {
	namespace detail {
		inline std::ostream& indent(std::ostream& os, size_t n) {
			while (n--) {
				print(os, "│ ");
			}

			return os;
		}
	}

	inline void printer_impl(Tree& tree, Tree::iterator current, Tree::iterator& it, size_t spaces = 0) {
		auto [str, kind] = *current;

		constexpr std::array colours { DECK_BLUE, DECK_YELLOW };
		const auto colour = colours[spaces % colours.size()];

		switch (kind) {
			case SymbolKind::String:  // Cases with associated string.
			case SymbolKind::Character:
			case SymbolKind::Integer:

			case SymbolKind::Word:
			case SymbolKind::Label:
			case SymbolKind::Address: {
				detail::indent(std::cerr, spaces);
				println(std::cerr, colour, kind, " ", str, DECK_RESET);
			} break;

			case SymbolKind::Header:  // No string data.
			case SymbolKind::Footer: {
				detail::indent(std::cerr, spaces);
				println(std::cerr, colour, kind, DECK_RESET);
			} break;

			case SymbolKind::Quote:  // Blocks
			case SymbolKind::Frame: {
				detail::indent(std::cerr, spaces);
				println(std::cerr, colour, kind, DECK_RESET);

				it = visit_block(printer_impl, tree, it, spaces + 1);

				detail::indent(std::cerr, spaces);
				println(std::cerr, colour, "End", DECK_RESET);
			} break;

			default: break;
		}
	}

	inline decltype(auto) printer(Tree&& tree) {
		DECK_LOG(Priority::Okay);

		pass(printer_impl, tree, 0ul);
		return std::move(tree);
	}
}  // namespace deck::passes

#endif
