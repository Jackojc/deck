#ifndef CDC_PASS_QUOTES_HPP
#define CDC_PASS_QUOTES_HPP

/*
	Make sure to include "../cdc.hpp" before this header.

	This pass canonicalises quotes (anonymous functions) into
	named functions at the global scope and replace the definition
	site with an address of the newly created function. This helps
	to keep things consistent and reduces the number of paths later
	passes need to take.
*/

namespace cdc {
	inline std::vector<Symbol>::iterator quotes_visit(
		Context& ctx,
		std::vector<Symbol>::iterator it,
		std::vector<Symbol>& instructions
	) {
		if (it == instructions.end())
			return it;

		CDC_LOG(LogLevel::INF, *it);

		std::vector<Symbol>::iterator current = it++;

		switch (current->kind) {
			case SymbolKind::QUOTE: {
				// Generate a unique name for this quote.
				std::string& id = ctx.intern.emplace_back("___fn_" + std::to_string(ctx.intern_id++));
				View sv { id.data(), id.data() + id.size() };

				// Change quote to address.
				current->kind = SymbolKind::ADDR;
				current->sv = sv;

				// Copy the quote symbol directly after.
				current = it = instructions.insert(current, *current) + 1;

				// Recurse and visit the body of the quote to find the end position.
				std::vector<Symbol>::iterator until = visit_block(ctx, it, instructions, quotes_visit);
				it = current;

				size_t length = std::distance(current, until);

				// Move quote to the end of the program and find new beginning and end.
				std::vector<Symbol>::iterator head = std::rotate(current, until, instructions.end() - 1);
				std::vector<Symbol>::iterator tail = head + length;

				tail--;

				// Change quote symbol to function symbol.
				head->kind = SymbolKind::FN;
				head->sv = sv;
				tail->sv = sv;
			} break;

			case SymbolKind::PROGRAM:
			case SymbolKind::MARK:
			case SymbolKind::LET:
			case SymbolKind::FN: {
				it = visit_block(ctx, it, instructions, quotes_visit);
			} break;

			default: break;
		}

		return it;
	}

	inline std::vector<Symbol>::iterator quotes(
		Context& ctx, std::vector<Symbol>& instructions
	) {
		return quotes_visit(ctx, instructions.begin(), instructions);
	}
}

#endif

