#ifndef CDC_PASS_EMIT_HPP
#define CDC_PASS_EMIT_HPP

/*
	Make sure to include "../cdc.hpp" before this header.

	This pass is a work in progress but eventually it will
	emit textual assembly to stdout which can then be passed
	into an assembler like NASM to be compiled to machine code.
*/

namespace cdc {
	inline std::vector<Symbol>::iterator emit_visit(
		Context& ctx,
		std::vector<Symbol>::iterator it,
		std::vector<Symbol>& instructions
	) {
		if (it == instructions.end())
			return it;

		CDC_LOG(LogLevel::INF, *it);

		std::vector<Symbol>::iterator current = it++;

		switch (current->kind) {
			case SymbolKind::BIND:
			case SymbolKind::ADDR:
			case SymbolKind::INT:
			case SymbolKind::STR:

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
			case SymbolKind::CALL: break;

			case SymbolKind::PROGRAM:
			case SymbolKind::MARK:
			case SymbolKind::LET:
			case SymbolKind::FN: {
				it = visit_block(ctx, it, instructions, emit_visit);
			} break;

			case SymbolKind::END:        // These node types are not valid at this stage.
			case SymbolKind::UNMARK:     // They should have been removed during earlier passes.
			case SymbolKind::UNQUOTE:
			case SymbolKind::QUOTE:
			case SymbolKind::IDENT: {
				report(current->sv, ErrorKind::UNREACHABLE);
			} break;

			default: break;
		}

		return it;
	}

	inline std::vector<Symbol>::iterator emit(
		Context& ctx, std::vector<Symbol>& instructions
	) {
		return emit_visit(ctx, instructions.begin(), instructions);
	}
}

#endif

