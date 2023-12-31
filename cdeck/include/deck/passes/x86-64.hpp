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

#include <fmt/core.h>

#include <deck/deck.hpp>

namespace deck::passes {
	namespace detail {
		struct X86Env {
			std::unordered_set<std::string> symbol_table;
			size_t id = 0;

			X86Env(): symbol_table { "+", "-", "*", "/", "%", "?", ".", "pop", "dup", "#", "clear" }, id { 0 } {}
		};

	}  // namespace detail

	template <typename... Ts>
	inline decltype(auto) emit(Ts&&... args) {
		println(std::cout, std::forward<Ts>(args)...);
	}

	inline void x86_64_primitive(std::string_view str, detail::X86Env& env) {
		using namespace std::literals;

		// Arithmetic
		if (str == "+"sv) {
			emit("  pop rbx");
			emit("  add rax, rbx");
		}

		else if (str == "-"sv) {
			emit("  pop rbx");
			emit("  sub rax, rbx");
		}

		else if (str == "*"sv) {
			emit("  pop rbx");
			emit("  imul rax, rbx");
		}

		else if (str == "/"sv) {
			emit("  pop rbx");
			emit("  div rbx");
		}

		else if (str == "%"sv) {
			emit("  pop rbx");
			emit("  div rbx");
			emit("  mov rax, rdx");
		}

		// Choice
		else if (str == "?"sv) {
			// False value is in rax.
			emit("  pop rbx");  // True value
			emit("  pop rcx");  // Condition value
			emit("  cmp rcx, 1");
			emit("  cmove rax, rbx");
		}

		else if (str == "."sv) {
			emit("  mov rbx, rax");
			emit("  pop rax");
			emit("  jmp rbx");
		}

		// Stack manipulation
		else if (str == "pop"sv) {
			emit("  pop rax");
		}

		else if (str == "dup"sv) {
			emit("  push rax");
		}

		else if (str == "#"sv) {
			emit("  push rax");
			emit("  mov rax, rbp");
			emit("  sub rbx, rsp");
			emit("  shr rax, 3");  // div 8
		}

		else if (str == "clear"sv) {
			emit("  mov rsp, rbp");
		}

		// Just call the function if it exists and isn't a primitive.
		else {
			size_t return_addr_id = env.id++;

			emit("  push rax");
			emit("  mov rax, __return_addr_", return_addr_id);
			emit("  jmp ", str);
			emit("__return_addr_", return_addr_id, ":");
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

				emit("section .text");
				emit("global _start");
				emit("_start:");
				emit("  mov rax, 0");
			} break;

			case SymbolKind::Footer: {
				// TODO: Emit exit syscall
			} break;

			// Literals
			case SymbolKind::String:
			case SymbolKind::Character: {
			} break;

			case SymbolKind::Integer: {
				emit("  push rax");
				emit("  mov rax, ", str);
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

				emit(str, ":");
			} break;

			case SymbolKind::Address: {
				// TODO: Check if label/extern exists and then push address.
				// We might also check if a primitive's address has been taken
				// here and emit a wrapper function for it so it can be addressed.

				if (auto it = env.symbol_table.find(str); it == env.symbol_table.end()) {
					fatal("`", str, "` is not defined");
				}

				emit("  push rax");
				emit("  mov rax, ", str);
			} break;

			// Anonymous function
			case SymbolKind::Quote: {
				size_t quote_id = env.id++;

				emit("  jmp __quote_end_", quote_id);
				emit("__quote_", quote_id, ":");

				it = visit_block(x86_64_impl, tree, it, env);

				emit("__quote_end_", quote_id, ":");

				emit("  push rax");
				emit("  mov rax, __quote_", quote_id);
			} break;

			// Stack frames
			case SymbolKind::Frame: {
				emit("  push rax");
				emit("  mov rax, rbp");
				emit("  mov rbp, rsp");

				it = visit_block(x86_64_impl, tree, it, env);

				emit("  mov rbp, rax");
				emit("  pop rax");
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
