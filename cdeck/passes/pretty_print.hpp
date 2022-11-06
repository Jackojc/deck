#ifndef CDC_PASS_PRETTY_PRINT_HPP
#define CDC_PASS_PRETTY_PRINT_HPP

/*
	Make sure to include "../cdc.hpp" before this header.

	This pass prints the tree to stdout for debugging
	purposes.
*/

namespace cdc {
	inline std::vector<Symbol>::iterator pretty_print_visit(
		Context& ctx,
		std::vector<Symbol>::iterator it,
		std::vector<Symbol>& instructions,
		size_t spaces = 0
	) {
		const auto indent = [&] {
			for (size_t i = 0; i != spaces; ++i)
				print(std::cout, "  ");
		};

		if (it == instructions.end())
			return it;

		std::vector<Symbol>::iterator current = it++;

		switch (current->kind) {
			case SymbolKind::END:        // These node types are not valid at this stage.
			case SymbolKind::UNMARK:     // They should have been removed during parsing.
			case SymbolKind::UNQUOTE: {
				report(current->sv, ErrorKind::UNREACHABLE);
			} break;

			// Literals.
			case SymbolKind::IDENT:
			case SymbolKind::BIND:
			case SymbolKind::ADDR:

			case SymbolKind::INT:
			case SymbolKind::STR: {
				indent(); println(std::cout, current->kind, " ", current->sv);
			} break;

			// Builtins
			case SymbolKind::CMP:

			case SymbolKind::NOT:
			case SymbolKind::XOR:
			case SymbolKind::OR:
			case SymbolKind::AND:

			case SymbolKind::SHR:
			case SymbolKind::SHL:

			case SymbolKind::MOD:
			case SymbolKind::DIV:
			case SymbolKind::MUL:
			case SymbolKind::SUB:
			case SymbolKind::ADD:

			case SymbolKind::SET:
			case SymbolKind::GET:

			case SymbolKind::POP:

			case SymbolKind::DEQ_POP:
			case SymbolKind::DEQ_PUSH:

			case SymbolKind::CHOOSE:
			case SymbolKind::BYTE:
			case SymbolKind::WORD:
			case SymbolKind::COUNT:
			case SymbolKind::GO:
			case SymbolKind::CALL: {
				indent(); println(std::cout, current->kind);
			} break;

			// Nested structures.
			case SymbolKind::PROGRAM:
			case SymbolKind::MARK:
			case SymbolKind::QUOTE:
			case SymbolKind::LET:
			case SymbolKind::FN: {
				indent(); println(std::cout, current->kind, " ", current->sv);
					it = visit_block(ctx, it, instructions, pretty_print_visit, spaces + 1);
				indent(); println(std::cout, "End");
			} break;

			default: break;
		}

		return it;
	}

	inline std::vector<Symbol>::iterator pretty_print(
		Context& ctx, std::vector<Symbol>& instructions
	) {
		return pretty_print_visit(ctx, instructions.begin(), instructions);
	}
}

#endif

