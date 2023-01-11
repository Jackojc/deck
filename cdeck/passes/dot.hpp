#ifndef CDC_PASS_DOT_HPP
#define CDC_PASS_DOT_HPP

/*
	Make sure to include "../cdc.hpp" before this header.

	This pass generates a dot graph which you can use to
	visualise control flow.
*/

namespace cdc {
	inline std::vector<Symbol>::iterator dot_visit(
		Context& ctx,
		std::vector<Symbol>::iterator it,
		std::vector<Symbol>& instructions,
		std::ostringstream& ss_block,
		size_t& id
	) {
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
				print(ss_block, current->kind, " ", current->sv, "\\l");
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
				print(ss_block, current->kind, "\\l");
			} break;

			// Nested structures.
			case SymbolKind::PROGRAM: {
				println(std::cout,
					"digraph G {\n"
					"  node [shape=record style=filled fontsize=10 fontname=\"Consolas\" fillcolor=\"#bfbfbf\"]\n"
					"  graph [style=filled fontsize=10 fontname=\"Consolas\" fillcolor=\"#efefef\" packmode=\"graph\"]\n"
					"  edge [fontsize=10 fontname=\"Consolas\" arrowsize=0.5]\n"
				);

				it = visit_block(ctx, it, instructions, dot_visit, ss_block, id);

				println(std::cout, "}");
			} break;

			case SymbolKind::FN: {
				println(std::cout, "  subgraph cluster_", id++, " {");
				println(std::cout, "    label=\"", current->sv, "\"");

				size_t fn_id = id++;

				it = visit_block(ctx, it, instructions, dot_visit, ss_block, id);

				std::string code = ss_block.str();

				if (code.empty())
					println(std::cout, "    \"n_", current->sv, "_", fn_id, "\" [label=\"\"]");
				else
					println(std::cout, "    \"n_", current->sv, "_", fn_id, "\" [label=\"{{", code, "}}\"]");

				ss_block.clear();
				println(std::cout, "  }");
			} break;

			case SymbolKind::MARK:
			case SymbolKind::QUOTE:
			case SymbolKind::LET: {
				println(std::cout, current->kind, " ", current->sv);
					it = visit_block(ctx, it, instructions, dot_visit, ss_block, id);
				println(std::cout, "End");
			} break;

			default: break;
		}

		return it;
	}

	inline std::vector<Symbol>::iterator dot(
		Context& ctx, std::vector<Symbol>& instructions
	) {
		size_t id = 0;
		std::ostringstream ss_block;
		return dot_visit(ctx, instructions.begin(), instructions, ss_block, id);
	}
}

#endif

