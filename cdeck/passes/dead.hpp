#ifndef CDC_PASS_DEAD_HPP
#define CDC_PASS_DEAD_HPP

/*
	Make sure to include "../cdc.hpp" before this header.

	This pass discovers which functions are never referenced and
	removes them from the source.
*/

namespace cdc {
	inline std::vector<Symbol>::iterator dead_visit(
		Context& ctx,
		std::vector<Symbol>::iterator it,
		std::vector<Symbol>& instructions,
		std::unordered_map<View, std::unordered_set<View>> inverse_refs
	) {
		if (it == instructions.end())
			return it;

		CDC_LOG(LogLevel::INF, *it);

		std::vector<Symbol>::iterator current = it++;

		switch (current->kind) {
			case SymbolKind::FN: {
				it = visit_block(ctx, it, instructions, dead_visit, inverse_refs);

				if (inverse_refs.find(current->sv) == inverse_refs.end()) {
					CDC_LOG(LogLevel::ERR, current->sv);
					// Erase function and make sure to update iterators.
				}
			} break;

			case SymbolKind::PROGRAM:
			case SymbolKind::MARK:
			case SymbolKind::QUOTE:
			case SymbolKind::LET: {
				it = visit_block(ctx, it, instructions, dead_visit, inverse_refs);
			} break;

			default: break;
		}

		return it;
	}

	inline std::vector<Symbol>::iterator dead(
		Context& ctx, std::vector<Symbol>& instructions
	) {
		// We generate an inverted reference graph here to get a "referenced by"
		// relationship between nodes. In other words, instead of the relationship
		// describing functions who reference other functions it is instead a
		// relationship of functions referenced by _other_ functions. This allows
		// us to easily identify functions not referenced by any others so we can
		// do dead function removal.
		std::unordered_map<View, std::unordered_set<View>> inverse_refs = {
			{ "main"_sv, {}} // We want to keep the main function always.
		};

		for (auto& [parent, children]: ctx.refs) {
			for (auto& child: children) {
				auto [it, succ] = inverse_refs.try_emplace(
					child, std::unordered_set<View> { parent }
				);

				if (not succ) {
					auto& [child, parents] = *it;
					parents.emplace(parent);
				}
			}
		}

		for (auto& [parent, children]: inverse_refs) {
			std::cout << parent;

			if (not children.empty())
				std::cout << ":\n  " << *children.begin();

			if (children.size() > 1)
				for (auto it = std::next(children.begin()); it != children.end(); ++it)
					std::cout << ", " << *it;

			std::cout << '\n';
		}

		return dead_visit(ctx, instructions.begin(), instructions, inverse_refs);
	}
}

#endif

