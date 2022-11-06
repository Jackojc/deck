#ifndef CDC_PASS_VERIFY_HPP
#define CDC_PASS_VERIFY_HPP

/*
	Make sure to include "../cdc.hpp" before this header.

	This pass verifies that references to either bindings or functions
	are in scope.
*/

namespace cdc {
	inline std::vector<Symbol>::iterator verify_visit(
		Context& ctx,
		std::vector<Symbol>::iterator it,
		std::vector<Symbol>& instructions
	) {
		if (it == instructions.end())
			return it;

		CDC_LOG(LogLevel::INF, *it);

		std::vector<Symbol>::iterator current = it++;

		switch (current->kind) {
			case SymbolKind::IDENT: {
				// Check if this symbol exists and, if so, change this identifier
				// to either an address or binding to match the definition.
				auto it = ctx.symbols.find(current->sv);

				if (it == ctx.symbols.end())
					report(current->sv, ErrorKind::UNDEFINED);

				switch (it->second) {
					case SymbolKind::FN:  current->kind = SymbolKind::ADDR; break;
					case SymbolKind::LET: current->kind = SymbolKind::BIND; break;
					default: break;
				}
			} break;

			case SymbolKind::LET: {
				// Add name to bindings ctx.
				auto [let_it, succ] = ctx.symbols.emplace(current->sv, SymbolKind::LET);

				if (not succ)
					report(current->sv, ErrorKind::CLASH);

				it = visit_block(ctx, it, instructions, verify_visit);

				// Remove name from bindings ctx.
				ctx.symbols.erase(current->sv);
			} break;

			case SymbolKind::PROGRAM:
			case SymbolKind::MARK:
			case SymbolKind::QUOTE:
			case SymbolKind::FN: {
				it = visit_block(ctx, it, instructions, verify_visit);
			} break;

			default: break;
		}

		return it;
	}

	inline std::vector<Symbol>::iterator verify(
		Context& ctx, std::vector<Symbol>& instructions
	) {
		return verify_visit(ctx, instructions.begin(), instructions);
	}
}

#endif

