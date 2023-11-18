#ifndef DECK_PASS_X86_64_HPP
#define DECK_PASS_X86_64_HPP

/*
	Emit x86-64 assembly
*/

#include <cstddef>
#include <cstdint>

#include <utility>
#include <iostream>

#include <array>
#include <unordered_map>
#include <string_view>
#include <string>

#include <deck/deck.hpp>

namespace deck::passes {
	namespace detail {
		struct X86Env {
			std::unordered_set<std::string> symbol_table;
			size_t id = 0;

			X86Env(): symbol_table { "+", "-", "*", "/", "%", "?", ".", "pop", "dup", "#", "clear" }, id { 0 } {}
		};
	}

	inline void x86_64_primitive(std::string_view str, detail::X86Env& env) {
		using namespace std::literals;

		// Arithmetic
		if (str == "+"sv) {
			println(std::cout, "  pop rbx");
			println(std::cout, "  add rax, rbx");
		}

		else if (str == "-"sv) {
			println(std::cout, "  pop rbx");
			println(std::cout, "  sub rax, rbx");
		}

		else if (str == "*"sv) {
			println(std::cout, "  pop rbx");
			println(std::cout, "  imul rax, rbx");
		}

		else if (str == "/"sv) {
			println(std::cout, "  pop rbx");
			println(std::cout, "  div rbx");
		}

		else if (str == "%"sv) {
			println(std::cout, "  pop rbx");
			println(std::cout, "  div rbx");
			println(std::cout, "  mov rax, rdx");
		}

		// Choice
		else if (str == "?"sv) {
			// False value is in rax.
			println(std::cout, "  pop rbx");  // True value
			println(std::cout, "  pop rcx");  // Condition value
			println(std::cout, "  cmp rcx, 1");
			println(std::cout, "  cmove rax, rbx");
		}

		else if (str == "."sv) {
			println(std::cout, "  mov rbx, rax");
			println(std::cout, "  pop rax");
			println(std::cout, "  jmp rbx");
		}

		// Stack manipulation
		else if (str == "pop"sv) {
			println(std::cout, "  pop rax");
		}

		else if (str == "dup"sv) {
			println(std::cout, "  push rax");
		}

		else if (str == "#"sv) {
			println(std::cout, "  push rax");
			println(std::cout, "  mov rax, rbp");
			println(std::cout, "  sub rbx, rsp");
			println(std::cout, "  shr rax, 3");  // div 8
		}

		else if (str == "clear"sv) {
			println(std::cout, "  mov rsp, rbp");
		}

		// Just call the function if it exists and isn't a primitive.
		else {
			println(std::cout, "  call ", str);
		}
	}

	inline void x86_64_impl(Tree& tree, Tree::iterator current, Tree::iterator& it, detail::X86Env& env) {
		auto [str, kind] = *current;

		switch (kind) {
			// ELF Metadata
			case SymbolKind::Header: {
				// TODO: Setup both main and return stacks.
				// Also make sure to take into account that the first push
				// will push garbage to the stack so we want to start 1 word
				// below the true starting point.

				println(std::cout, "section .text");
				println(std::cout, "global _start");
				println(std::cout, "_start:");
				println(std::cout, "  mov rax, 0");
			} break;

			case SymbolKind::Footer: {
				// TODO: Emit exit syscall
			} break;

			// Literals
			case SymbolKind::String:
			case SymbolKind::Character: {
			} break;

			case SymbolKind::Integer: {
				println(std::cout, "  push rax");
				println(std::cout, "  mov rax, ", str);
			} break;

			// Function call
			case SymbolKind::Identifier: {
				if (auto it = env.symbol_table.find(str); it == env.symbol_table.end()) {
					fatal("`", str, "` is not defined");
				}

				x86_64_primitive(str, env);
			} break;

			// Definition and address
			case SymbolKind::Declare: {
				// TODO: Emit an extern directive. Should we move all of these
				// to the top of the emitted assembly?

				if (auto [it, succ] = env.symbol_table.emplace(str); not succ) {
					fatal("`", str, "` is declared already");
				}
			} break;

			case SymbolKind::Label: {
				if (auto [it, succ] = env.symbol_table.emplace(str); not succ) {
					fatal("`", str, "` is declared already");
				}

				println(std::cout, str, ":");
			} break;

			case SymbolKind::Address: {
				// TODO: Check if label/extern exists and then push address.
				// We might also check if a primitive's address has been taken
				// here and emit a wrapper function for it so it can be addressed.

				if (auto it = env.symbol_table.find(str); it == env.symbol_table.end()) {
					fatal("`", str, "` is not defined");
				}

				println(std::cout, "  push rax");
				println(std::cout, "  mov rax, ", str);
			} break;

			// Anonymous function
			case SymbolKind::Quote: {
				size_t quote_id = env.id++;

				println(std::cout, "  jmp __quote_end_", quote_id);
				println(std::cout, "__quote_", quote_id, ":");

				it = visit_block(x86_64_impl, tree, it, env);

				println(std::cout, "__quote_end_", quote_id, ":");

				println(std::cout, "  push rax");
				println(std::cout, "  mov rax, __quote_", quote_id);
			} break;

			// Stack frames
			case SymbolKind::Frame: {
				println(std::cout, "  push rax");
				println(std::cout, "  mov rax, rbp");
				println(std::cout, "  mov rbp, rsp");

				it = visit_block(x86_64_impl, tree, it, env);

				println(std::cout, "  mov rbp, rax");
				println(std::cout, "  pop rax");
			} break;

			case SymbolKind::End: break;

			default: {
				DECK_LOG(Priority::Warn, "unhandled symbol: `", kind, "`");
			} break;
		}
	}

	inline Tree x86_64(Tree&& tree) {
		DECK_LOG(Priority::Okay);

		detail::X86Env env;
		pass(x86_64_impl, tree, env);

		for (const std::string& str: env.symbol_table) {
			DECK_LOG(Priority::Info, str);
		}

		return tree;
	}
}  // namespace deck::passes

#endif
