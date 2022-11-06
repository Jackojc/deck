#ifndef CDC_PASS_REORDER_HPP
#define CDC_PASS_REORDER_HPP

/*
	Make sure to include "../cdc.hpp" before this header.

	This pass generates a reference graph to discover which
	functions reference which other functions. This is similar
	to a call graph with the difference being that function calls
	are a runtime thing in Deck and so it's entirely possible to
	reference a function and then not call it.
*/

namespace cdc {
	// void dfs(
	// 	std::unordered_map<View, std::unordered_set<View>>& graph,
	// 	View parent,
	// 	std::vector<View>& order,
	// 	std::unordered_set<View>& seen
	// ) {
	// 	if (auto it = graph.find(parent); it != graph.end()) {
	// 		for (View child: it->second) {
	// 			if (parent == child)
	// 				continue;

	// 			dfs(graph, child, order, seen);

	// 			if (seen.find(child) == seen.end())
	// 				order.emplace_back(child);

	// 			seen.emplace(child);
	// 		}
	// 	}
	// }

	inline std::vector<Symbol>::iterator reorder_visit(
		Context& ctx,
		std::vector<Symbol>::iterator it,
		std::vector<Symbol>& instructions,
		std::vector<Symbol>& new_instructions,
		std::unordered_set<View>& seen
	) {
		if (it == instructions.end())
			return it;

		CDC_LOG(LogLevel::INF, *it);

		std::vector<Symbol>::iterator current = it++;

		switch (current->kind) {
			case SymbolKind::ADDR: {
				// Check if we've seen this function before already. If so, we return early.
				// If not, we lookup the function corresponding to the current reference and
				// then visit its body recursively.
				if (seen.find(current->sv) != seen.end())
					return it;

				if (auto it = ctx.ranges.find(current->sv); it != ctx.ranges.end()) {
					auto [begin, end] = it->second;
					visit_block(ctx, begin, instructions, reorder_visit, new_instructions, seen);
				}
			} break;

			case SymbolKind::FN: {
				// Return early if we've already seen this. If not, we add it to the seen
				// functions and then visit the body. After recursively visiting the body,
				// we copy the function over to the new tree.
				if (seen.find(current->sv) != seen.end())
					return it;

				seen.emplace(current->sv);
				it = visit_block(ctx, it, instructions, reorder_visit, new_instructions, seen);

				if (auto it = ctx.ranges.find(current->sv); it != ctx.ranges.end()) {
					auto [begin, end] = it->second;
					new_instructions.insert(new_instructions.end(), begin, end);
				}
			} break;

			case SymbolKind::PROGRAM:
			case SymbolKind::MARK:
			case SymbolKind::QUOTE:
			case SymbolKind::LET: {
				it = visit_block(ctx, it, instructions, reorder_visit, new_instructions, seen);
			} break;

			default: break;
		}

		return it;
	}

	inline std::vector<Symbol>::iterator reorder(
		Context& ctx, std::vector<Symbol>& instructions
	) {
		std::unordered_set<View> seen;
		std::vector<Symbol> new_instructions = { instructions.front() };

		auto it = reorder_visit(ctx, instructions.begin(), instructions, new_instructions, seen);

		new_instructions.emplace_back(instructions.back());

		instructions = new_instructions;
		return it;
	}

	// inline std::vector<Symbol>::iterator reorder(
	// 	Context& ctx, std::vector<Symbol>& instructions
	// ) {
	// 	std::vector<View> order;
	// 	std::unordered_set<View> seen;

	// 	std::vector<Symbol> new_instructions;

	// 	for (auto& [parent, set]: ctx.refs) {
	// 		dfs(ctx.refs, parent, order, seen);

	// 		if (seen.find(parent) == seen.end())
	// 			order.emplace_back(parent);

	// 		seen.emplace(parent);
	// 	}

	// 	for (View fn: order) {
	// 		if (auto it = ctx.ranges.find(fn); it != ctx.ranges.end()) {
	// 			auto [begin, end] = it->second;
	// 			new_instructions.insert(new_instructions.end(), begin, end);
	// 		}
	// 	}
	// }
}

#endif

