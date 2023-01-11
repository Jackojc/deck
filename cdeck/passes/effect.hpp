#ifndef CDC_PASS_EFFECT_HPP
#define CDC_PASS_EFFECT_HPP

/*
	Make sure to include "../cdc.hpp" before this header.

	This pass tries to discover statically known stack
	effects and exploit them as optimisation oppurtunities.
*/

namespace cdc {

	// Stack effect checking pass to discover how many inputs and outputs functions
	// have. We can build up from primitives and then store the effects of user defined
	// functions and eventually have a stack effect for every function.

	inline std::vector<Symbol>::iterator effect_visit(
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
				it = visit_block(ctx, it, instructions, effect_visit);
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

	inline std::vector<Symbol>::iterator effect(
		Context& ctx, std::vector<Symbol>& instructions
	) {
		return effect_visit(ctx, instructions.begin(), instructions);
	}
}

#endif

