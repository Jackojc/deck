#ifndef DECK_PASS_DUMPER_HPP
#define DECK_PASS_DUMPER_HPP

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
	inline void dumper_impl(Tree&, Tree::iterator current, Tree::iterator&) {
		auto [str, kind] = *current;

		switch (kind) {
			case SymbolKind::String:  // Cases with associated string.
			case SymbolKind::Character:
			case SymbolKind::Integer:
			case SymbolKind::Identifier:

			case SymbolKind::Declare:
			case SymbolKind::Label:
			case SymbolKind::Address: {
				println(std::cerr, kind, " ", str, DECK_RESET);
			} break;

			case SymbolKind::Header:  // No string data.
			case SymbolKind::Footer: {
				println(std::cerr, kind, DECK_RESET);
			} break;

			case SymbolKind::Quote:  // Blocks
			case SymbolKind::Frame: {
				println(std::cerr, kind, DECK_RESET);
			} break;

			case deck::SymbolKind::End: {
				println(std::cerr, kind, DECK_RESET);
			} break;

			default: {
				DECK_LOG(Priority::Warn, "unhandled symbol: `", kind, "`");
			} break;
		}
	}

	inline Tree dumper(Tree&& tree) {
		DECK_LOG(Priority::Okay);

		pass(dumper_impl, tree);
		return tree;
	}
}  // namespace deck::passes

#endif
