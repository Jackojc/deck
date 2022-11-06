#ifndef CDC_PASS_DISCOVER_HPP
#define CDC_PASS_DISCOVER_HPP

/*
	Make sure to include "../cdc.hpp" before this header.

	This pass records all function definitions so that we can
	reference functions out of order and avoid having to use
	forward declarations like C.
*/

namespace cdc {
	inline std::vector<Symbol>::iterator discover_visit(
		Context& ctx,
		std::vector<Symbol>::iterator it,
		std::vector<Symbol>& instructions
	) {
		if (it == instructions.end())
			return it;

		CDC_LOG(LogLevel::INF, *it);

		const std::vector<Symbol>::iterator current = it++;

		switch (current->kind) {
			case SymbolKind::FN: {
				// Add the function name to the symbol table.
				auto [fn_it, succ] = ctx.symbols.emplace(current->sv, SymbolKind::FN);

				// If this symbol already exists, we have a naming conflict.
				if (not succ)
					report(current->sv, ErrorKind::CLASH);

				it = visit_block(ctx, it, instructions, discover_visit);
			} break;

			case SymbolKind::PROGRAM:
			case SymbolKind::MARK:
			case SymbolKind::QUOTE:
			case SymbolKind::LET: {
				it = visit_block(ctx, it, instructions, discover_visit);
			} break;

			default: break;
		}

		return it;
	}

	inline std::vector<Symbol>::iterator discover(
		Context& ctx, std::vector<Symbol>& instructions
	) {
		return discover_visit(ctx, instructions.begin(), instructions);
	}
}

#endif

