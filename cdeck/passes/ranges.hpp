#ifndef CDC_PASS_RANGES_HPP
#define CDC_PASS_RANGES_HPP

/*
	Make sure to include "../cdc.hpp" before this header.

	This pass records the iterator pair that encompass a function
	for all functions in the program so that we can perform operations
	involving the entire body of a function (i.e. inlining).
*/

namespace cdc {
	inline std::vector<Symbol>::iterator ranges_visit(
		Context& ctx,
		std::vector<Symbol>::iterator it,
		std::vector<Symbol>& instructions
	) {
		if (it == instructions.end())
			return it;

		CDC_LOG(LogLevel::INF, *it);

		std::vector<Symbol>::iterator current = it++;

		switch (current->kind) {
			case SymbolKind::FN: {
				// The `try_emplace` here can't fail because at this point
				// we should know that there are no naming conflicts between
				// functions.
				it = visit_block(ctx, it, instructions, ranges_visit);
				ctx.ranges.try_emplace(current->sv, current, it);
			} break;

			case SymbolKind::PROGRAM:
			case SymbolKind::MARK:
			case SymbolKind::QUOTE:
			case SymbolKind::LET: {
				it = visit_block(ctx, it, instructions, ranges_visit);
			} break;

			default: break;
		}

		return it;
	}

	inline std::vector<Symbol>::iterator ranges(
		Context& ctx, std::vector<Symbol>& instructions
	) {
		return ranges_visit(ctx, instructions.begin(), instructions);
	}
}

#endif

