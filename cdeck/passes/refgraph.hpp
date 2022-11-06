#ifndef CDC_PASS_REFGRAPH_HPP
#define CDC_PASS_REFGRAPH_HPP

/*
	Make sure to include "../cdc.hpp" before this header.

	This pass generates a reference graph to discover which
	functions reference which other functions. This is similar
	to a call graph with the difference being that function calls
	are a runtime thing in Deck and so it's entirely possible to
	reference a function and then not call it.
*/

namespace cdc {
	inline std::vector<Symbol>::iterator refgraph_visit(
		Context& ctx,
		std::vector<Symbol>::iterator it,
		std::vector<Symbol>& instructions,
		View parent = View {}  // Parent function.
	) {
		if (it == instructions.end())
			return it;

		CDC_LOG(LogLevel::INF, *it);

		std::vector<Symbol>::iterator current = it++;

		switch (current->kind) {
			case SymbolKind::ADDR: {
				// Try to insert this relationship of parent -> child if an
				// entry for the parent doesn't already exist. If the parent
				// has already been seen then we need to instead emplace the
				// child into the existing set.
				auto [ref_it, succ] = ctx.refs.try_emplace(
					parent, std::unordered_set<View> { current->sv }
				);

				if (not succ) {
					auto& [parent, children] = *ref_it;
					children.emplace(current->sv);
				}
			} break;

			case SymbolKind::FN: {
				// We pass down the new parent here using `current->sv`.
				it = visit_block(ctx, it, instructions, refgraph_visit, current->sv);
			} break;

			case SymbolKind::PROGRAM:
			case SymbolKind::MARK:
			case SymbolKind::QUOTE:
			case SymbolKind::LET: {
				it = visit_block(ctx, it, instructions, refgraph_visit, parent);
			} break;

			default: break;
		}

		return it;
	}

	inline std::vector<Symbol>::iterator refgraph(
		Context& ctx, std::vector<Symbol>& instructions
	) {
		return refgraph_visit(ctx, instructions.begin(), instructions);
	}
}

#endif

