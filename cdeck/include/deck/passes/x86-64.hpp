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
			size_t id;

			X86Env(): id { 0 } {}
		};
	}

	inline void x86_64_primitive(std::string_view str, detail::X86Env& env) {
		using namespace std::literals;

		// Arithmetic
		if (str == "+"sv) {
			println(std::cout, "pop rbx");
			println(std::cout, "add rax, rbx");
		}

		else if (str == "-"sv) {
			println(std::cout, "pop rbx");
			println(std::cout, "sub rax, rbx");
		}

		else if (str == "*"sv) {
			println(std::cout, "pop rbx");
			println(std::cout, "imul rax, rbx");
		}

		else if (str == "/"sv) {
			println(std::cout, "pop rbx");
			println(std::cout, "div rbx");
		}

		else if (str == "%"sv) {
			println(std::cout, "pop rbx");
			println(std::cout, "div rbx");
			println(std::cout, "mov rax, rdx");
		}

		else {
			println(std::cout, "call ", str);
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
				println(std::cout, "mov rax, 0");
			} break;

			case SymbolKind::Footer: {
				// TODO: Emit exit syscall
			} break;

			// Literals
			case SymbolKind::String:
			case SymbolKind::Character: {
			} break;

			case SymbolKind::Integer: {
				println(std::cout, "push rax");
				println(std::cout, "mov rax, ", str);
			} break;

			// Function call
			case SymbolKind::Word: {
				// TODO: Check word exists.
				x86_64_primitive(str, env);
			} break;

			// Definition and address
			case SymbolKind::Declare: {
				// TODO: Emit an extern directive. Should we move all of these
				// to the top of the emitted assembly?
			} break;

			case SymbolKind::Label: {
				// TODO: Insert name to symbol table.
			} break;

			case SymbolKind::Address: {
				// TODO: Check if label/extern exists and then push address.
				// We might also check if a primitive's address has been taken
				// here and emit a wrapper function for it so it can be addressed.
			} break;

			// Anonymous function
			case SymbolKind::Quote: {
				size_t quote_id = env.id++;

				println(std::cout, "jmp __quote_end_", quote_id);
				println(std::cout, "__quote_", quote_id, ":");

				it = visit_block(x86_64_impl, tree, it, env);

				println(std::cout, "__quote_end_", quote_id, ":");

				println(std::cout, "push rax");
				println(std::cout, "mov rax, __quote_", quote_id);
			} break;

			// Stack frames
			case SymbolKind::Frame: {
				println(std::cout, "push rax");
				println(std::cout, "mov rax, rbp");
				println(std::cout, "mov rbp, rsp");

				it = visit_block(x86_64_impl, tree, it, env);

				println(std::cout, "mov rbp, rax");
				println(std::cout, "pop rax");
			} break;

			default: break;
		}
	}

	inline decltype(auto) x86_64(Tree&& tree) {
		DECK_LOG(Priority::Okay);

		detail::X86Env env;
		pass(x86_64_impl, tree, env);

		return std::move(tree);
	}
}  // namespace deck::passes

#endif
